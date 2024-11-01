/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_readline.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:15:39 by nbardavi          #+#    #+#             */
/*   Updated: 2024/11/01 16:31:30 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_readline.h"
#include "42sh.h"
#include "ft_regex.h"
#include "c_string.h"
#include "libft.h"
#include "utils.h"
#include <linux/limits.h>
#include <stdio.h>
#include <termios.h>
#include <termcap.h>
#include <unistd.h>
#include <ncurses.h> 
#include <sys/ioctl.h>
#include <sys/time.h>


//TODO:Limiter history -> HIST_SIZE
//HistFileSize HistFile
//ctrl + v multiline

int rl_done = 0;

void move_cursor(int x, int y);
void set_cursor_position(readline_state_t *rl_state);
void signal_prompt_mode(void);

void set_prompt(readline_state_t *rl_state, const char *new_prompt) {
	gc(GC_FREE, rl_state->prompt.data, GC_READLINE);
	rl_state->prompt = string_init_str(new_prompt);
	gc(GC_ADD, rl_state->prompt.data, GC_READLINE);
}

size_t get_col(void){
	struct winsize	w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col;
}

void rl_print_prompt(int fd, readline_state_t *rl_state){
	write(fd, rl_state->prompt.data, rl_state->prompt.size);
}

size_t get_row(void) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_row;
}

void enable_raw_mode()
{
    struct termios raw;

    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode()
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

//TODO:Organiser ca, fonction donne envie de se pendre la
void update_cursor_x(readline_state_t *rl_state, string *line, ssize_t n) {
    size_t cols = get_col();
	size_t rows = get_row();
    ssize_t new_cursor_x = rl_state->cursor.x + n;
    int tchars = line->size + rl_state->prompt.size;  //total chars
    int nlines = tchars / cols; //number lines

    if (new_cursor_x < 0) {//back
        ssize_t lines_up = (-new_cursor_x + cols - 1) / cols;
        rl_state->cursor.y -= lines_up;

        new_cursor_x = cols + new_cursor_x - ((rl_state->cursor.y == 0) ? rl_state->prompt.size : 0);
        rl_state->cursor.x = (size_t)new_cursor_x;
    } else if (rl_state->cursor.y == 0 && new_cursor_x + rl_state->prompt.size >= cols){
		rl_state->cursor.y++;
		rl_state->cursor.x = 0;
	}
    else if ((size_t)new_cursor_x >= cols){//newline
        rl_state->cursor.y = rl_state->cursor.y + new_cursor_x / cols;
        rl_state->cursor.x = new_cursor_x % cols;
    } 
    else {//normal
        rl_state->cursor.x = (size_t)new_cursor_x;
    }
	if (rl_state->cursor.y + (int)rl_state->cursor_offset.y >= (int)rows){
		rl_state->cursor_offset.y = rows - 1 - ((nlines > 0) ? nlines : 1);
		write(STDOUT_FILENO, "\n", 1);
	}
}

readline_state_t *manage_rl_state(manage_rl_state_mode mode, readline_state_t *new_rl_state){
	static readline_state_t *rl_state = NULL;
	if (mode == RL_GET){
		return rl_state;
	} else if (mode == RL_SET){
		rl_state = new_rl_state;
	}
	return NULL;
}

void ft_rl_newline() {
	readline_state_t *rl_state = manage_rl_state(RL_GET, NULL);

    write(1, "^C", 2);
	write(1, "\n", 1);
	rl_state->search_mode.active = false;
	rl_state->cursor_offset.y++;
	rl_state->cursor_offset.x = 0;
	rl_state->cursor.y = 0;
	rl_state->cursor.x = 0;
	rl_state->prompt.size = ft_strlen(rl_state->prompt.data);
	rl_print_prompt(STDOUT_FILENO, rl_state);
}

void move_cursor(int x, int y) {
    char buffer[2048];
    char *area = buffer;
    char *move_cursor_seq = tgetstr("cm", &area);
	
    if (move_cursor_seq) {
        char *cursor_position = tgoto(move_cursor_seq, x, y);
        tputs(cursor_position, 1, putchar);
        fflush(stdout);
    }
}

void set_cursor_position(readline_state_t *rl_state) {
    int adjusted_x = rl_state->cursor.x + (rl_state->cursor.y == 0 ? rl_state->prompt.size + rl_state->cursor_offset.x: 0);
    move_cursor(adjusted_x, rl_state->cursor.y + rl_state->cursor_offset.y);
}

void ft_readline_clean(){
	readline_state_t *rl_state = manage_rl_state(RL_GET, NULL);
	
	(void)rl_state;
	gc(GC_FREE, rl_state->prompt.data, GC_READLINE);
	destroy_history();
	gc(GC_FREE, rl_state, GC_READLINE);
}

void get_cursor_pos(position_t *position){
    char buf[32];
    unsigned int i = 0;
    int rows = 0, cols = 0;
    int parse_mode = 0;

    write(STDOUT_FILENO, "\033[6n", 4);

    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1 || buf[i] == 'R') {
            break;
        }
        i++;
    }
    buf[i] = '\0';

    if (buf[0] == '\033' && buf[1] == '[') {
        for (i = 2; buf[i] != '\0'; i++) {
            if (buf[i] == ';') {
                parse_mode = 1;
                continue;
            }
            if (buf[i] == 'R') {
                break;
            }

            if (parse_mode == 0) {
                rows = rows * 10 + (buf[i] - '0');
            } else {
                cols = cols * 10 + (buf[i] - '0');
            }
        }
		position->y = rows;
        position->x = cols;
    } else {
        position->y = 0;
        position->x = 0;
    }
}

void update_line(readline_state_t *rl_state, string *line) {
	// int cols = get_col();
 //    int tchars = line->size + rl_state->prompt.size;
    // int nlines = tchars / cols;

	move_cursor(rl_state->cursor_offset.x, rl_state->cursor_offset.y);
	write(STDOUT_FILENO, "\033[0J", 4);
	// for (int i = 0; i <= nlines; i++){
	// 	write(STDOUT_FILENO, "\033[0K", 4);
	// 	move_cursor(rl_state->prompt.size, rl_state->cursor_offset.y + i + 1);
	// }
	if (rl_state->search_mode.active == false){
		move_cursor(rl_state->cursor_offset.x, rl_state->cursor_offset.y);
		rl_print_prompt(STDOUT_FILENO, rl_state);
		write(STDOUT_FILENO, line->data, line->size);
	} else {
		move_cursor(0, rl_state->cursor_offset.y);
		if (line->data[0]){
			rl_state->search_mode.word_found = search_in_history(line->data);
		}
		if (!rl_state->search_mode.word_found){
			rl_state->prompt.size = sizeof("(failed reverse-i-search)`") - 1;
			write(STDOUT_FILENO, "(failed reverse-i-search)`", 27);
		}
		else{
			rl_state->prompt.size = sizeof("(reverse-i-search)`") - 1;
			write(STDOUT_FILENO, "(reverse-i-search)`", 20);
		}
		write(STDOUT_FILENO, line->data, str_length(line));
		write(STDOUT_FILENO, "':", 2);
		write(STDOUT_FILENO, rl_state->search_mode.word_found, ft_strlen(rl_state->search_mode.word_found));
	}
}

void init_readline(readline_state_t *rl_state, const char *prompt){
    char buffer[2048] = {0};

    set_prompt(rl_state, prompt);
	tgetent(buffer, getenv("TERM"));

	//TODO:definir history autrement
	if (!history_defined) {
		init_history();
		history_defined = true;
	}
	history->offset = history->length;
	add_history("");

	enable_raw_mode();

	get_cursor_pos(&rl_state->cursor_offset);
	rl_state->cursor_offset.y--; //rm line break
	rl_state->cursor_offset.x--; // same problem
	move_cursor(rl_state->cursor_offset.x, rl_state->cursor_offset.y);
	rl_print_prompt(STDOUT_FILENO, rl_state);
	manage_rl_state(RL_SET, rl_state);
}

int handle_normal_keys(readline_state_t *rl_state, char c, string *line){
	
	int pos = rl_state->cursor.y * get_col() - rl_state->prompt.size;
	pos += rl_state->cursor.x + ((rl_state->cursor.y == 0) ? rl_state->prompt.size : 0);

	if (c == '\n' || c == '\0'){
		if (rl_state->search_mode.active){
			rl_state->search_mode.active = false;
			rl_state->prompt.size = ft_strlen(rl_state->prompt.data);
			if (rl_state->search_mode.word_found){
				gc(GC_FREE, line->data, GC_READLINE);
				*line = string_init_str(rl_state->search_mode.word_found);
				gc(GC_ADD, line->data, GC_READLINE);
			}
			update_line(rl_state, line);
		}
		if (rl_state->interactive){
			int nrows = get_row();
			if (rl_state->cursor_offset.y + rl_state->cursor.y == nrows - 1) {
				write(STDOUT_FILENO, "\n", 1);
				rl_state->cursor_offset.y = nrows - 1;
			} else {
				rl_state->cursor_offset.y++;
			}
			rl_state->cursor.y = 0;
			rl_state->cursor.x = 0;
			move_cursor(0, rl_state->cursor_offset.y + rl_state->cursor.y);
		}
		return 1;
	}

    if (c == 127 && !pos){
        return 2;
    }

    if (rl_state->interactive || (pos == str_length(line))){
        if (c == 127) {
            str_pop_back(line);
			update_cursor_x(rl_state, line, -2);
        }
        else {
            str_push_back(line, c);
        }
    } else {
        if (c == 127) {
            str_erase(line, pos - 1, 1);
			update_cursor_x(rl_state, line, -2);
        } else {
            str_insert(line, c, pos);
        }
    }
	if (rl_state->interactive)
		update_cursor_x(rl_state, line, 1);
    return 0;
}

int can_go_left(readline_state_t *rl_state){
	if (rl_state->cursor.y == 0)
		return rl_state->cursor.x > 0;
	else
		return rl_state->cursor.x > -(int)rl_state->prompt.size;
}

int can_go_right(readline_state_t *rl_state, string *line) {
    int cols = get_col(); //number collums
    int tchars = line->size + rl_state->prompt.size;  //total chars
    int nlines = tchars / cols; //number lines
    int nchar_on_last_line = tchars % cols; 

    if (rl_state->cursor.y == 0) {
        if (nlines >= 1) {
            return rl_state->cursor.x < (int)(cols - rl_state->prompt.size);
        } else {
            return rl_state->cursor.x < (int)(nchar_on_last_line - rl_state->prompt.size);
        }
    }
    
    if (rl_state->cursor.y == nlines) {
        return rl_state->cursor.x < nchar_on_last_line;
    } 
    
    return rl_state->cursor.x < cols;
}

int should_process_enter() {
	static struct timeval last_enter_time = {0};
    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    long time_diff = (current_time.tv_sec - last_enter_time.tv_sec) * 1000 + 
                     (current_time.tv_usec - last_enter_time.tv_usec) / 1000;

    if (time_diff > 75){
        last_enter_time = current_time;
        return 1;
    }
    return 0;
}


char *ft_readline(const char *prompt) {
	readline_state_t *rl_state = NULL;
	
	if (manage_rl_state(RL_GET, NULL)){
		rl_state = manage_rl_state(RL_GET, NULL);
	} else {
		rl_state = gc_unique(readline_state_t, GC_READLINE);
		rl_state->cursor.x = 0;
		rl_state->cursor.y = 0;
		rl_state->cursor_offset.x = 0;
		rl_state->cursor_offset.y = 0;

		rl_state->search_mode.active = 0;
		rl_state->search_mode.word_found = NULL;	

		rl_state->prompt = string_init_str("");
		gc(GC_ADD, rl_state->prompt.data, GC_READLINE);
	}

	
	ShellInfos *self = shell(SHELL_GET);
    rl_state->interactive = self->interactive;

    if (rl_state->interactive)
		init_readline(rl_state, prompt);

	string *line = gc(GC_ALLOC, 1, sizeof(string), GC_READLINE);
    if (rl_state->interactive) {
        *line = str_strdup(&history->entries[history->length - 1]->line);
		gc(GC_ADD, line->data, GC_READLINE);
    } else {
        *line = STRING_L("");
		gc(GC_ADD, line->data, GC_READLINE);
    }
    rl_done = 0;

    do {
		char c = '\0';
        ssize_t bytes_read = read(STDIN_FILENO, &c, 1);
		//maybe rl_set_position

		if (c == '\n' && !should_process_enter()) {
			continue;
		}

		if (rl_done){
			ft_rl_newline();
			rl_done = 0;
			str_clear(line);
			continue; }

		int result = 0;
        if (bytes_read == 0 || c == VEOF) {
            if (line->data[0] == '\0') {
                if (rl_state->interactive) {
                    write(STDOUT_FILENO, "\n", 1);
                    pop_history();
                }
                // gc(GC_FREE, line->data, GC_READLINE);
				// gc(GC_FREE, line, GC_READLINE);
                return NULL;
            }
		} else if (c == 12 || c == 18) { // ^L | ^R
			handle_control_keys(rl_state, c);
        } else if (c == '\033') {
            result = handle_special_keys(rl_state, line);
        } else {
			result = handle_normal_keys(rl_state, c, line);
        }
		
		if (result == 1) {
			break;
		}
		
		if (rl_state->interactive) {
			update_line(rl_state, line);
			set_cursor_position(rl_state);
		}
    } while (true);
    
    if (rl_state->interactive)
        disable_raw_mode();
    
    char *str = NULL;
	if (!rl_done)
		str = ft_strdup(line->data);
	else{
		str = ft_strdup("");
	}
	
	gc(GC_FREE, line->data, GC_READLINE);
	gc(GC_FREE, line, GC_READLINE);

    if (rl_state->interactive)
        pop_history();
    
    return str;
}

