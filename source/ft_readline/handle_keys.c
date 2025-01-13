/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_keys.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 11:37:52 by bvan-pae          #+#    #+#             */
/*   Updated: 2025/01/13 16:07:28 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_readline.h"
#include "ft_regex.h"
#include "c_string.h"
#include "lexer.h"
#include "libft.h"
#include "utils.h"
#include "dynamic_arrays.h"

#include <limits.h>
#include <stdio.h>
#include <termios.h>
#include <termcap.h>
#include <unistd.h>
#include <ncurses.h> 
#include <sys/ioctl.h>
#include <sys/time.h>


//TODO: Clean handle_enter_function

int last_action = -1;

void go_left(readline_state_t *rl_state, string *line){
	if (can_go_left(rl_state)){
		update_cursor_x(rl_state, line, -1);
		set_cursor_position(rl_state);
	}
}

void go_right(readline_state_t *rl_state, string *line){
	if (can_go_right(rl_state, line)) {
		update_cursor_x(rl_state, line, 1);
		set_cursor_position(rl_state);
	}
}

int handle_enter_key(readline_state_t *rl_state, string *line) {
	if (rl_state->interactive){
		if (rl_state->search_mode.active){
			rl_state->search_mode.active = false;
			rl_state->prompt_size = ft_strlen(rl_state->prompt);
			if (rl_state->search_mode.word_found){
				gc(GC_FREE, line->data, GC_READLINE);
				*line = string_init_str(rl_state->search_mode.word_found);
				gc(GC_ADD, line->data, GC_READLINE);
			}
			update_line(rl_state, line);
		}

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
	return RL_REFRESH;
}

void handle_backspace_key(readline_state_t *rl_state, string *line, size_t pos){
	if (!pos)
		return;
	str_erase(line, pos - 1, 1);
	update_cursor_x(rl_state, line, -1);
}

void rl_swap_char(readline_state_t *rl_state, string *line){
    if (line->size < 2) return;

    char current = rl_get_current_char(rl_state, line);
    char prev = rl_get_prev_char(rl_state, line);
    char next = rl_get_next_char(rl_state, line);
    if (prev == '\02'){
        rl_change_current_char(rl_state, line, rl_get_n_char(rl_state, line, 1));
        rl_change_n_char(rl_state, line, current, 1);
        update_cursor_x(rl_state, line, 2);
    } else if (next == '\03'){
        rl_change_prev_char(rl_state, line, rl_get_n_char(rl_state, line, -2));
        rl_change_n_char(rl_state, line, prev, -2);
    } else {
        rl_change_current_char(rl_state, line, prev);
        rl_change_prev_char(rl_state, line, current);
        update_cursor_x(rl_state, line, 1);
    }
}

bool is_alnum(const char c){
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'));
}

typedef struct s_word_pos {
    int curr_word_end;
    int curr_word_start;
    int prev_word_end;
    int prev_word_start;
} word_pos_t;

static void get_words_pos(readline_state_t *rl_state, string *line, word_pos_t *pos){
    // char current_char;
    int it = rl_get_cursor_pos_on_line(rl_state);
    
    for (;line->data[it] && !is_alnum(line->data[it]); it++){}
    for (;line->data[it] && is_alnum(line->data[it]); it++){}
    it--;
    pos->curr_word_end = it;
    for (;it > 0 && is_alnum(line->data[it]); it--){}
    pos->curr_word_start = it + 1;
    for (;it > 0 && !is_alnum(line->data[it]); it--){}
    pos->prev_word_end = it;

    for (;it > 0 && is_alnum(line->data[it]); it--){}
    pos->prev_word_start = (it == 0) ? it : it + 1;

    // move_cursor(0, 0);
    // dprintf(2, "DEBUG: word_pos_t values:\n");
    // dprintf(2, "string: %s\n", line->data);
    // dprintf(2, "  curr_word_start: %d\n", pos->curr_word_start);
    // dprintf(2, "  curr_word_end:   %d\n", pos->curr_word_end);
    // dprintf(2, "  prev_word_start: %d\n", pos->prev_word_start);
    // dprintf(2, "  prev_word_end:   %d\n", pos->prev_word_end);
    // fflush(stderr);
    // set_cursor_position(rl_state);
}

void rl_swap_word(readline_state_t *rl_state, string *line){
    if (line->size == 0) return;

    word_pos_t pos;
    get_words_pos(rl_state, line, &pos);

    
    int prev_len = pos.prev_word_end - pos.prev_word_start + 1;
    int curr_len = pos.curr_word_end - pos.curr_word_start + 1;
    if (pos.prev_word_end == pos.prev_word_start) return;
    
    string curr_word = str_substr(line, pos.curr_word_start, curr_len);
    string prev_word = str_substr(line, pos.prev_word_start, prev_len);

    str_erase(line, pos.prev_word_start, prev_len);
    str_erase(line, pos.curr_word_start - prev_len, curr_len);

    str_insert_str(line, curr_word.data, pos.prev_word_start);
    str_insert_str(line, prev_word.data, pos.prev_word_start + curr_len + (pos.curr_word_start - pos.prev_word_end - 1));

    // examples'   'nathan
    // '   '
    // nathan'   '
    // nathan'  'examples

    
    // move_cursor(0, 0);
    // dprintf(2, "prev_word: %20s|len: %d\n", prev_word.data, prev_len);
    // dprintf(2, "curr_word: %20s|len: %d\n", curr_word.data, curr_len);
    // dprintf(2, "line: %s|\n", line->data);
    // fflush(stderr);
    // set_cursor_position(rl_state);
    
    rl_state->cursor.x = 0;
    rl_state->cursor.y = 0;
    update_cursor_x(rl_state, line, pos.curr_word_end + 1);
    // rl_state->cursor.x = pos.curr_word_start + prev_len;
}

int ft_isnotspace(int c){
    return (c != ' ');
}

void handle_vi_control(readline_state_t *rl_state, char c, string *line){
    switch (c){
        case 'j':
            rl_go_down(rl_state, line); break;
        case 'k':
            rl_go_up(rl_state); break;
        case 'i':
            rl_state->in_line.vi_mode = VI_INSERT; break;
        case 'h':
            go_left(rl_state, line); break;
        case 'l':
            go_right(rl_state, line); break;
        case 'w':
            go_right_word(rl_state, line, &ft_isalnum); break;
        case 'W':
            go_right_word(rl_state, line, &ft_isnotspace); break;
        case 'b':
            go_left_word(rl_state, line, &ft_isalnum); break;
        case 'B':
            go_left_word(rl_state, line, &ft_isnotspace); break;
    }
}

int handle_printable_keys(readline_state_t *rl_state, char c, string *line){
    // rl_save_undo_state(line, rl_state);
	int pos = rl_state->cursor.y * get_col() - rl_state->prompt_size;
	pos += rl_state->cursor.x + ((rl_state->cursor.y == 0) ? rl_state->prompt_size : 0);

	if (c == '\n' || c == '\0'){
		return handle_enter_key(rl_state, line);
	}

    if (rl_state->in_line.mode == RL_VI && rl_state->in_line.vi_mode == VI_NORMAL){
        handle_vi_control(rl_state, c, line);
        return RL_NO_OP;
    }

	if (c == 127 && rl_state->interactive){
        if (last_action != DELETE_KEY && rl_state->interactive){
            rl_save_undo_state(line, rl_state);
            last_action = DELETE_KEY;
        }
		handle_backspace_key(rl_state, line, pos);
		return RL_NO_OP;
	}

    if (last_action != PRINT_KEY && rl_state->interactive){
        rl_save_undo_state(line, rl_state);
        last_action = PRINT_KEY;
    }
	
	if (!rl_state->interactive){
		str_push_back(line, c);
	}
	else {
		/*dprintf(2, "line: %s\n", line->data);*/
		str_insert(line, c, pos);
	}

	if (rl_state->interactive)
		update_cursor_x(rl_state, line, 1);

    return RL_NO_OP;
}

void handle_delete_key(readline_state_t *rl_state, string *line) {
    int pos = rl_state->cursor.y * get_col() - rl_state->prompt_size;
    pos += rl_state->cursor.x + ((rl_state->cursor.y == 0) ? rl_state->prompt_size : 0);

    if (pos >= str_length(line)) return;

    str_erase(line, pos, 1);

    if (rl_state->interactive) {
        set_cursor_position(rl_state);
    }
}


rl_event up_history(readline_state_t *rl_state, string *line, Vars *shell_vars) {
	(void)shell_vars;
	if (history->navigation_offset < history->length - CURRENT_LINE){ //-1
		history->navigation_offset++;
		rl_state->cursor.x = history->entries[history->length - history->navigation_offset - 1]->line.size;
		gc(GC_FREE, line->data, GC_READLINE);
		*line = str_strdup(&history->entries[history->length - history->navigation_offset - 1]->line);
		gc(GC_ADD, line->data, GC_READLINE);
	}
	// move_cursor(0, 0);
	// print_history_values(history);
	// set_cursor_position(rl_state);
	return RL_NO_OP;
}

rl_event down_history(readline_state_t *rl_state, string *line) {
	if (history->navigation_offset > 0){
		history->navigation_offset--;
		rl_state->cursor.x = history->entries[history->length - history->navigation_offset - 1]->line.size;
		gc(GC_FREE, line->data, GC_READLINE);
		*line = str_strdup(&history->entries[history->length - history->navigation_offset - 1]->line);
		gc(GC_ADD, line->data, GC_READLINE);
	}
	return RL_NO_OP;
}

void go_end(readline_state_t *rl_state, string *line){
    while(can_go_right(rl_state, line)){
        go_right(rl_state, line);
    }
}

void go_start(readline_state_t *rl_state, string *line){
    while(can_go_left(rl_state)){
        go_left(rl_state, line);
    }
}

void go_right_word(readline_state_t *rl_state, string *line, int (*compare_func)(int)) {
    while (can_go_right(rl_state, line) && !compare_func(rl_get_current_char(rl_state, line))){
        go_right(rl_state, line);
    }
    char next_char = rl_get_current_char(rl_state, line);
    while (can_go_right(rl_state, line) && compare_func(next_char) && next_char != '\0') {
        go_right(rl_state, line);
        next_char = rl_get_current_char(rl_state, line);
    } 
    while (can_go_right(rl_state, line) && !compare_func(rl_get_current_char(rl_state, line))){
        go_right(rl_state, line);
    }
}

void go_left_word(readline_state_t *rl_state, string *line, int (*compare_func)(int)) {
    while (can_go_left(rl_state) && !compare_func(rl_get_prev_char(rl_state, line))) {
        go_left(rl_state, line);
    }

    char prev_char = rl_get_prev_char(rl_state, line);
    while (can_go_left(rl_state) && compare_func(prev_char) && prev_char != '\0') {
        go_left(rl_state, line);
        prev_char = rl_get_prev_char(rl_state, line);
    }
}

rl_event handle_readline_controls(readline_state_t *rl_state, char c, string *line, Vars *shell_vars){
    if (rl_state->interactive)
		rl_save_undo_state(line, rl_state);
    if (rl_state->in_line.mode == RL_READLINE){
        switch (c) {
            case '\02': // <C> + b
                go_left(rl_state, line); break;
            case '\06':
                go_right(rl_state, line); break;
            case '\020':
                up_history(rl_state, line, shell_vars); break;
            case '\016':
                down_history(rl_state, line); break;
            case '\05': // <C> + e
                go_end(rl_state, line); break;
            case '\01': // <C> + a
                go_start(rl_state, line); break;
            case '\037': // <C> + _
                da_pop(rl_state->undo_stack);
                rl_load_previous_state(line, rl_state); break;
            case '\024':
                rl_swap_char(rl_state, line); break;
            default: {
                da_pop(rl_state->undo_stack);
                return RL_NO_OP;
            }
        }
    }
    return RL_NO_OP;
}
rl_event handle_special_keys(readline_state_t *rl_state, string *line, Vars *shell_vars) {
    char seq[3];

    fd_set set;
    struct timeval timeout = { 0, 20 };
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);

    int selret = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);

    if (selret == -1) {
        perror("select error");
        return RL_NO_OP;
    } else if (selret == 0) {
        if (rl_state->in_line.mode == RL_VI) {
            rl_state->in_line.vi_mode = VI_NORMAL; 
        }
        return RL_NO_OP;
    }

    if (read(STDIN_FILENO, &seq[0], 1) != 1) {
        return RL_REFRESH;
    }

    if (seq[0] == '[') {
        if (read(STDIN_FILENO, &seq[1], 1) != 1) {
            return RL_REFRESH;
        }

        switch (seq[1]) {
            case 'A': // Flèche haut
                return up_history(rl_state, line, shell_vars);
            case 'B': // Flèche bas
                return down_history(rl_state, line);
            case 'C': // Flèche droite
                go_right(rl_state, line);
                break;
            case 'D': // Flèche gauche
                go_left(rl_state, line);
                break;
            default:
                break;
        }
    } else if (rl_state->in_line.mode == RL_READLINE) {
        if (seq[0] == 'b') {
            go_left_word(rl_state, line, &ft_isalnum);
        } else if (seq[0] == 'f') {
            go_right_word(rl_state, line, &ft_isalnum);
        } else if (seq[0] == 't') {
            rl_swap_word(rl_state, line);
        }
    }

    return RL_NO_OP;
}

void handle_control_keys(readline_state_t *rl_state, char char_c){
	if (char_c == CTRL_L){
		write(STDOUT_FILENO, "\033[2J", 4);
		rl_state->cursor.y = 0;
		rl_state->cursor_offset.y = 0;
		rl_state->cursor_offset.x = 0;
		move_cursor(0, 0);
		rl_print_prompt(STDOUT_FILENO, rl_state);
	}
	if (char_c == CTRL_R){
		rl_state->search_mode.active = true;
	}
}
