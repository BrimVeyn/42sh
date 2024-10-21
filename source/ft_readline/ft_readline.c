/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_readline.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 11:26:40 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/21 16:01:37 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_readline.h"
#include "42sh.h"
#include "ft_regex.h"
#include "c_string.h"
#include "libft.h"
#include <linux/limits.h>
#include <readline/keymaps.h>
#include <stdio.h>
#include <termios.h>
#include <termcap.h>
#include <unistd.h>
#include <ncurses.h> 
#include <sys/ioctl.h>

int rl_done = 0;

void move_cursor(int x, int y);
void set_cursor_position();
void signal_prompt_mode(void);

typedef struct s_readline_state {
    char *prompt;
	size_t offset_prompt;
	size_t offset_x;
	size_t offset_y;
    int cursor_x;
    int cursor_y;
} readline_state_t;

readline_state_t rl_state = {NULL, 0, 0, 0, 0, 0};

void set_prompt(const char *new_prompt) {
    if (rl_state.prompt)
        free(rl_state.prompt);
    rl_state.prompt = strdup(new_prompt);
}

char *get_prompt() {
    return rl_state.prompt;
}

size_t get_col(void){
	struct winsize	w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col;
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
void update_cursor_x(string *line, ssize_t n) {
    size_t cols = get_col();
	size_t rows = get_row();
    ssize_t new_cursor_x = rl_state.cursor_x + n;
    int tchars = line->size + rl_state.offset_x;  //total chars
    int nlines = tchars / cols; //number lines

    if (new_cursor_x < 0) {//back
        ssize_t lines_up = (-new_cursor_x + cols - 1) / cols;
        rl_state.cursor_y -= lines_up;

        new_cursor_x = cols + new_cursor_x - ((rl_state.cursor_y == 0) ? rl_state.offset_x : 0);
        rl_state.cursor_x = (size_t)new_cursor_x;
    } else if (rl_state.cursor_y == 0 && new_cursor_x + rl_state.offset_x >= cols){
		rl_state.cursor_y++;
		rl_state.cursor_x = 0;
	}
    else if ((size_t)new_cursor_x >= cols){//newline
        rl_state.cursor_y = rl_state.cursor_y + new_cursor_x / cols;
        rl_state.cursor_x = new_cursor_x % cols;
    } 
    else {//normal
        rl_state.cursor_x = (size_t)new_cursor_x;
    }
	if (rl_state.cursor_y + (int)rl_state.offset_y >= (int)rows){
		rl_state.offset_y = rows - 1 - ((nlines > 0) ? nlines : 1);
		write(1, "\n", 1);
	}
}

void ft_rl_newline() {
    write(1, "^C", 2);
	write(1, "\n", 1);
	rl_state.offset_y++;
	rl_state.offset_prompt = 0;
	rl_state.offset_x = ft_strlen(get_prompt());
	rl_state.cursor_y = 0;
	rl_state.cursor_x = 0;
	write(1, get_prompt(), ft_strlen(get_prompt()));
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

void set_cursor_position() {
    int adjusted_x = rl_state.cursor_x + (rl_state.cursor_y == 0 ? rl_state.offset_x : 0);
    move_cursor(adjusted_x, rl_state.cursor_y + rl_state.offset_y);
}

void ft_readline_clean(){
	if (rl_state.prompt){
		free(rl_state.prompt);
	}
	destroy_history();
}

void get_cursor_pos(){
    char buf[32];
    unsigned int i = 0;
	
    write(STDOUT_FILENO, "\033[6n", 4);

    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1 || buf[i] == 'R') {
            break;
        }
        i++;
    }
    buf[i] = '\0';
    if (buf[0] == '\033' && buf[1] == '[') {
        regex_match_t result = regex_match("\\[[0-9]*;", buf);
        char *tmp_y = ft_substr( buf, result.re_start + 1, result.re_end - result.re_start);
        rl_state.offset_y = ft_atoi(tmp_y);
        free(tmp_y);
		
        result = regex_match(";[0-9]*$", buf);
        char *tmp_x = ft_substr( buf, result.re_start + 1, result.re_end - result.re_start);
        rl_state.offset_prompt = ft_atoi(tmp_x);
        free(tmp_x);
    }
}

void update_line(string *line) {
	(void)line;
	int cols = get_col();
    int tchars = line->size + rl_state.offset_x;
    int nlines = tchars / cols;

	move_cursor(rl_state.offset_x, rl_state.offset_y);
	write(1, "\033[0K", 4);
	for (int i = rl_state.cursor_y; i <= nlines; i++) {
		write(1, "\033[0K", 4);
		move_cursor(0, rl_state.offset_y + i + 1);
	}
	move_cursor(rl_state.offset_x, rl_state.offset_y);
	// write(1, get_prompt(), ft_strlen(get_prompt()));
	if (line->size){
		write(1, line->data, line->size);
	}
}

//TODO:change offset_x (echo -n)
void init_readline(const char *prompt){
    char buffer[2048];

    set_prompt(prompt);
	tgetent(buffer, getenv("TERM"));

	if (!history_defined) {
		init_history();
		history_defined = true;
	}

	history->offset = history->length;
	add_history("");

	enable_raw_mode();

	get_cursor_pos();
	rl_state.offset_y--;
	rl_state.offset_prompt--;
	move_cursor(rl_state.offset_prompt, rl_state.offset_y);
	write(STDOUT_FILENO, get_prompt(), ft_strlen(get_prompt()));
	rl_state.offset_x = rl_state.offset_prompt + ft_strlen(get_prompt());
}

int handle_normal_keys(char c, string *line, int interactive){

	int pos = rl_state.cursor_y * get_col() - rl_state.offset_x;
	pos += rl_state.cursor_x + ((rl_state.cursor_y == 0) ? rl_state.offset_x : 0);

    if (c == '\n' || c == '\0'){
		if (interactive){
			write(1, "\n\033[2K\r", 6);
			rl_state.offset_y++;
			rl_state.cursor_y = 0;
			rl_state.cursor_x = 0;
		}
        return 1;
    }
    if (c == 127 && !pos){
        return 2;
    }

    if (!interactive || (pos == str_length(line))){
        if (c == 127) {
            str_pop_back(line);
			update_cursor_x(line, -2);
        }
        else {
            str_push_back(line, c);
        }
    } else {
        if (c == 127) {
            str_erase(line, pos - 1, 1);
			update_cursor_x(line, -2);
        } else {
            str_insert(line, c, pos);
        }
    }
    update_cursor_x(line, 1);
    return 0;
}

int can_go_left(void){
	if (rl_state.cursor_y == 0)
		return rl_state.cursor_x > 0;
	else
		return rl_state.cursor_x > -(int)rl_state.offset_x;
}

int can_go_right(string *line) {
    int cols = get_col(); //number collums
    int tchars = line->size + rl_state.offset_x;  //total chars
    int nlines = tchars / cols; //number lines
    int nchar_on_last_line = tchars % cols; 

    if (rl_state.cursor_y == 0) {
        if (nlines >= 1) {
            return rl_state.cursor_x < (int)(cols - rl_state.offset_x);
        } else {
            return rl_state.cursor_x < (int)(nchar_on_last_line - rl_state.offset_x);
        }
    }
    
    if (rl_state.cursor_y == nlines) {
        return rl_state.cursor_x < nchar_on_last_line;
    } 
    
    return rl_state.cursor_x < cols;
}

int handle_special_keys(string *line){
    char seq[2];
    if (read(STDIN_FILENO, &seq[0], 1) == 0) return 1;
    if (read(STDIN_FILENO, &seq[1], 1) == 0) return 1;

    if (seq[0] == '['){
        if (seq[1] == 'A' && history->offset > 0){//left
            history->offset--;
            rl_state.cursor_x = history->entries[history->offset]->line.size;
			str_destroy(line);
            *line = str_strdup(&history->entries[history->offset]->line);
            return 2;
        }
        else if (seq[1] == 'D' && can_go_left()){
			update_cursor_x(line, -1);
			set_cursor_position();
        }
        else if (seq[1] == 'C' && can_go_right(line)){
			update_cursor_x(line, 1);
			set_cursor_position();
        }
        else if (seq[1] == 'B' && history->offset < history->length - 1){
            history->offset++;
            rl_state.cursor_x = history->entries[history->offset]->line.size;
			str_destroy(line);
            *line = str_strdup(&history->entries[history->offset]->line);
            return 2;
        }
    }
    return 0;
}

void handle_control_keys(char char_c){
	if (char_c == 12){
		write(STDOUT_FILENO, "\033[2J", 4);
		rl_state.cursor_y = 0;
		rl_state.offset_y = 0;
		rl_state.offset_prompt = 0;
		rl_state.offset_x = ft_strlen(get_prompt());
		move_cursor(0, 0);
		write(STDOUT_FILENO, get_prompt(), ft_strlen(get_prompt()));
	}
	if (char_c == 18){
		char c = 0;
		// string word = STRING_L("");
		do {
			ssize_t bytes_read = read(STDIN_FILENO, &c, 1);

			if (bytes_read == 0 || c == VEOF) {}//exit
			
			
		} while (c != '\n');
	}
}

#include <time.h>

char *ft_readline(const char *prompt) {
	// clock_t start = clock();
    char c = '\0';
    string *line = NULL;
    int result = 0;
    const int interactive = isatty(STDIN_FILENO);

    if (interactive)
        init_readline(prompt);

    line = malloc(sizeof(string));
    if (interactive) {
        *line = str_strdup(&history->entries[history->length - 1]->line);
    } else {
        *line = STRING_L("");
    }

    rl_done = 0;
    do {
	
        ssize_t bytes_read = read(STDIN_FILENO, &c, 1);
		// move_cursor(0,1);
		// printf("%d\n", c);
		// set_cursor_position();

		if (rl_done) {
			ft_rl_newline();
			rl_done = 0;
			continue;
		}

        if ( bytes_read == 0 || c == VEOF) {
            if (line->data[0] == '\0') {
                if (interactive) {
                    write(1, "\n", 1);
                    pop_history();
                }
                str_destroy(line);
                free(line);
                return NULL;
            }
		} else if (c == 12 || c == 18){ //^L | ^R
			handle_control_keys(c);
        } else if (c == '\033') {
            result = handle_special_keys(line);
        } else {
            result = handle_normal_keys(c, line, interactive);
        }

        if (result == 1)
			break;

        if (interactive){
				update_line(line);
			set_cursor_position();
        }
    } while (true);
    
    if (interactive)
        disable_raw_mode();
    
    char *str = NULL;
    if (!rl_done)
        str = ft_strdup(line->data);
    
    str_destroy(line);
    free(line);
    
    if (interactive)
        pop_history();
    
    if (rl_done) {
        return ft_strdup("");
    }
	// clock_t end = clock();
	// double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
	// printf("Time spent in read(): %f seconds\n", time_spent);
    
    return str;
}

//TODO:add calloc in garbage collector
//TODO:$PS1

