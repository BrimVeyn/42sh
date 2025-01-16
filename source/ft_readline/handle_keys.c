/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_keys.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 11:37:52 by bvan-pae          #+#    #+#             */
/*   Updated: 2025/01/16 14:55:16 by nbardavi         ###   ########.fr       */
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

int handle_enter_key(readline_state_t *rl_state, string *line) {
	if (rl_state->interactive){
        // reset prompt if in another mode than normal
        if (rl_manage_args(RL_GET, 0)){
            rl_manage_args(RL_RESET, 0);
            set_prompt(rl_state, rl_state->normal_prompt);
            update_line(rl_state, line);
        }
		if (rl_state->search_mode.active){
			rl_state->current_prompt_size = ft_strlen(rl_state->current_prompt);
            rl_state->search_mode.active = false;
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

static int ft_isnotspace(int c){
    return (c != ' ');
}

int rl_manage_args(manage_rl_accessor mode, int n){
    static int number = 0;

    if (mode == RL_SET){
        number *= 10;
        number += n;
    } else if (mode == RL_RESET){
        number = 0;
    }

    if (number > SHRT_MAX){
        number = 0;
    }
    return number;
}

void switch_to_insert_mode(readline_state_t *rl_state){
    rl_state->in_line.vi_mode = VI_INSERT;
    rl_manage_args(RL_RESET, 0);
}

void rl_launch_superior_mods(readline_state_t *rl_state, string *line, void (*superior_func)(readline_state_t, string)){
    char c = 0;

    while(waiting_for_){
        read(STDIN_FILENO, &c, 1);
        handle
    }
}

void handle_vi_control(readline_state_t *rl_state, char c, string *line){
    
    int args = rl_manage_args(RL_GET, 0);
    if (c >= '1' && c <= '9'){
        args = rl_manage_args(RL_SET, c - '0');
        return;
    }
    args = (!args) ? 1 : args;
    switch (c){
        case 'j':
            rl_repeat_by_args(rl_state, line, &down_history, args) ; break;
        case 'k':
            rl_repeat_by_args(rl_state, line, &up_history, args); break;
        case 'i':
            switch_to_insert_mode(rl_state); rl_move_back_by_char(rl_state, line); break;
        case 'I':
            switch_to_insert_mode(rl_state); rl_move_to_start(rl_state, line); break;
        case 'a':
            switch_to_insert_mode(rl_state); break;
        case 'A':
            switch_to_insert_mode(rl_state); rl_move_to_end(rl_state, line); break;
        case 'l':
            rl_repeat_by_args(rl_state, line, &rl_move_forward_by_char, args); break;
        case 'h':
            rl_repeat_by_args(rl_state, line, &rl_move_back_by_char, args); break;
        case 'w':
            rl_repeat_by_args_with_comp(rl_state, line, &ft_isalnum, &rl_move_to_next_word_start, args); break;
        case 'W':
            rl_repeat_by_args_with_comp(rl_state, line, &ft_isnotspace, &rl_move_to_next_word_start, args); break;
        case 'b':
            rl_repeat_by_args_with_comp(rl_state, line, &ft_isalnum, &rl_move_to_previous_word_start, args); break;
        case 'B':
            rl_repeat_by_args_with_comp(rl_state, line, &ft_isnotspace, &rl_move_to_previous_word_start, args); break;
        case '0':
            rl_move_to_start(rl_state, line);break;
        case '$':
            rl_move_to_end(rl_state, line);break;
        case '^':
            rl_move_to_first_char(rl_state, line);break;
        case 'e':
            rl_repeat_by_args_with_comp(rl_state, line, &ft_isalnum, &rl_move_to_next_word_end, args); break;
        case 'E':
            rl_repeat_by_args_with_comp(rl_state, line, &ft_isnotspace, &rl_move_to_next_word_end, args); break;
        case '|':
            rl_move_to_n_index(rl_state, line, args); break;
        case 'f':
            rl_move_to_next_matching_char(rl_state, line, args, RL_NEWMATCH); break;
        case 'F':
            rl_move_to_prev_matching_char(rl_state, line, args, RL_NEWMATCH); break;
        case ';':
            rl_manage_matching_vi_mode(NULL, RL_GET)(rl_state, line, args, RL_REMATCH); break;
        case 'c':
            rl_change_until_next_key_pressed(rl_state, line); break;
        case 'C':
            rl_change_until_end(rl_state, line); break;
        case 's':
            rl_repeat_by_args(rl_state, line, rl_substitute_current_char, args); break;
        case 'S':
            rl_substitute_line(rl_state, line); break;
        default:
            return;
    }
    rl_manage_args(RL_RESET, 0);
}

int handle_printable_keys(readline_state_t *rl_state, char c, string *line){
    // rl_save_undo_state(line, rl_state);
	int pos = rl_state->cursor.y * get_col() - rl_state->current_prompt_size;
	pos += rl_state->cursor.x + ((rl_state->cursor.y == 0) ? rl_state->current_prompt_size : 0);

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
    int pos = rl_state->cursor.y * get_col() - rl_state->current_prompt_size;
    pos += rl_state->cursor.x + ((rl_state->cursor.y == 0) ? rl_state->current_prompt_size : 0);

    if (pos >= str_length(line)) return;

    str_erase(line, pos, 1);

    if (rl_state->interactive) {
        set_cursor_position(rl_state);
    }
}

rl_event handle_readline_controls(readline_state_t *rl_state, char c, string *line){
    if (rl_state->interactive)
		rl_save_undo_state(line, rl_state);
    if (rl_state->in_line.mode == RL_READLINE){
        switch (c) {
            case '\02': // <C> + b
                rl_move_back_by_char(rl_state, line); break;
            case '\06':
                rl_move_forward_by_char(rl_state, line); break;
            case '\020':
                up_history(rl_state, line); break;
            case '\016':
                down_history(rl_state, line); break;
            case '\05': // <C> + e
                rl_move_to_end(rl_state, line); break;
            case '\01': // <C> + a
                rl_move_to_start(rl_state, line); break;
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

rl_event handle_special_keys(readline_state_t *rl_state, string *line) {
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
                up_history(rl_state, line); break;
            case 'B': // Flèche bas
                down_history(rl_state, line); break;
            case 'C': // Flèche droite
                rl_move_forward_by_char(rl_state, line); break;
            case 'D': // Flèche gauche
                rl_move_back_by_char(rl_state, line); break;
                break;
            default:
                break;
        }
    } else if (rl_state->in_line.mode == RL_READLINE) {
        if (seq[0] == 'b') {
            rl_move_to_previous_word_start(rl_state, line, &ft_isalnum);
        } else if (seq[0] == 'f') {
            rl_move_to_next_word_start(rl_state, line, &ft_isalnum);
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

//Pet18005651[]
