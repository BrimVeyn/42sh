/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_keys.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 11:37:52 by bvan-pae          #+#    #+#             */
/*   Updated: 2025/01/14 16:56:22 by nbardavi         ###   ########.fr       */
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

static int ft_isnotspace(int c){
    return (c != ' ');
}

void handle_vi_control(readline_state_t *rl_state, char c, string *line){
    switch (c){
        case 'j':
            down_history(rl_state, line); break;
        case 'k':
            up_history(rl_state, line); break;
        case 'i':
            rl_state->in_line.vi_mode = VI_INSERT; break;
        case 'h':
            rl_move_forward_by_char(rl_state, line); break;
        case 'l':
            rl_move_forward_by_char(rl_state, line); break;
        case 'w':
            rl_move_to_next_word_start(rl_state, line, &ft_isalnum); break;
        case 'W':
            rl_move_to_next_word_start(rl_state, line, &ft_isnotspace); break;
        case 'b':
            rl_move_to_previous_word_start(rl_state, line, &ft_isalnum); break;
        case 'B':
            rl_move_to_previous_word_start(rl_state, line, &ft_isnotspace); break;
        case '0':
            rl_move_to_start(rl_state, line);break;
        case '$':
            rl_move_to_end(rl_state, line);break;
        case '^':
            rl_move_to_first_char(rl_state, line);break;
        case 'e':
            rl_move_to_next_word_end(rl_state, line, &ft_isalnum); break;
        case 'E':
            rl_move_to_next_word_end(rl_state, line, &ft_isnotspace); break;
        
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
                return up_history(rl_state, line);
            case 'B': // Flèche bas
                return down_history(rl_state, line);
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
