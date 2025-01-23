/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_keys.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 11:37:52 by bvan-pae          #+#    #+#             */
/*   Updated: 2025/01/23 14:45:52 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "ft_readline.h"
#include "ft_regex.h"
#include "c_string.h"
#include "lexer.h"
#include "libft.h"
#include "utils.h"
#include "dynamic_arrays.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>
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

bool leader_pressed_twice = false;
bool had_a_movement = false;


bool rl_isMoveKey(char c, rl_movement_func **func) {
    switch (c) {
        case 'j': { *func = &down_history; return true; }
        case 'k': { *func = &up_history; return true; }
        case 'l': { *func = &rl_move_forward_by_char; return true; }
        case 'h': { *func = &rl_move_back_by_char; return true; }
        case ' ': { *func = &rl_move_forward_by_char; return true; }
        case 'w': { *func = &rl_move_to_next_word_start_alnum; return true; }
        case 'W': { *func = &rl_move_to_next_word_start_sp; return true; }
        case 'b': { *func = &rl_move_to_previous_word_start_sp; return true; }
        case 'B': { *func = &rl_move_to_previous_word_start_alnum; return true; }
        case 'e': { *func = &rl_move_to_next_word_end_alnum; return true; }
        case 'E': { *func = &rl_move_to_next_word_end_sp; return true; }
        case '0': { *func = &rl_move_to_start; return true; }
        case '$': { *func = &rl_move_to_end; return true; }
        case '^': { *func = &rl_move_to_first_char; return true; }
        case 'p': { *func = &rl_paste_after_cursor; return true; }
        case 'P': { *func = &rl_paste_on_cursor; return true; }
        case '|': { *func = &rl_move_to_n_index; return true; }
        case 'f': { *func = &rl_handle_find_next_key; return true; }
        case 'F': { *func = &rl_handle_find_prev_key; return true; }
        case ';': { *func = &rl_handle_redo_previous_match; return true; }
        case ',': { *func = &rl_handle_redo_reverse_match; return true; }
        case 'r': { *func = &rl_replace_current_char; return true; }
        default: { return false; }
    }
    return false;
}

bool rl_isNumberKey(char c, int *arg){
    // move_cursor(0, 0);
    // dprintf(2, "c: %c\n", c);
    if (('0' < c && c <= '9') || (arg && c == '0')){
        // dprintf(2, "salut\n");
        *arg *= 10;
        *arg += (c - '0');

        if (*arg > SHRT_MAX){
            *arg = 0;
        }
        return true;
    }
    return false;
}

static void rl_dummy_leader(readline_state_t *rl_state, string *line, size_t n){
    (void)rl_state; (void)line; (void)n;
}

static void rl_change_keybind(readline_state_t *rl_state, string *line, size_t n){
    rl_delete_from_n_to_cursor(rl_state, line, n);
    switch_to_insert_mode(rl_state);
}

static void rl_delete_keybind(readline_state_t *rl_state,string *line, size_t n){
    rl_delete_from_n_to_cursor(rl_state, line, n);
}

bool rl_isLeaderKey(char c, rl_leader_func **func) {
    switch (c) {
        case 'c': { *func = &rl_change_keybind; return true; };
        case 'y': { *func = &rl_dummy_leader; return true; };
        case 'd': { *func = &rl_delete_keybind; return true; };
        default: return false;
    }
}

void switch_to_replace_mode(readline_state_t *rl_state){
    rl_state->in_line.vi_mode = VI_REPLACE;
    rl_state->in_line.arg = 0;
}

void rl_open_line_in_editor(readline_state_t *rl_state, string *line, Vars *shell_vars){
    char filename[] = "/tmp/42sh_rl.XXXXXX"; //7 digit max for usec
    int fd = mkstemp(filename);
    if (fd == -1){
        return;
    }
    if (line->size)
        dprintf(fd, "%s", line->data);
    char *input = NULL;
	input = gc(GC_CALLOC, ft_strlen(filename) + sizeof("${EDITOR} ") + 1, sizeof(char), GC_SUBSHELL);
    ft_sprintf(input, "${EDITOR} %s", filename);
	parse_input(input, NULL, shell_vars);
    line->data = gc(GC_ADD, read_whole_file(fd), GC_SUBSHELL);
    printf("\n%s\n", line->data);
    rl_state->in_line.exec_line = true;
}

static bool rl_is_changing_mode_key(readline_state_t *rl_state, string *line, char c, Vars *shell_vars){
    switch (c){
        case 'i': { switch_to_insert_mode(rl_state); return true; }
        case 'I': { switch_to_insert_mode(rl_state); rl_move_to_start(rl_state, line); return true; }
        case 'a': { switch_to_insert_mode(rl_state); rl_move_forward_by_char(rl_state, line); return true;}
        case 'A': { switch_to_insert_mode(rl_state); rl_move_to_end(rl_state, line); return true; }
        case 'R': { switch_to_replace_mode(rl_state); return true; }
        case 'u': { rl_load_previous_state(line, rl_state); return true; }
        case 'v': { rl_open_line_in_editor(rl_state, line, shell_vars); return true; }
        default: return false;
    }
}

void handle_vi_control(readline_state_t *rl_state, char c, string *line, rl_vi_controls_mode mode, Vars *shell_vars){
    (void)mode;
    struct {
        int state;
        char c;
        rl_leader_func *leader_func;
        rl_movement_func *movement_func;
        int arg;
        bool need_read;
    } ctx = {
        .state = 0,
        .c = c,
        .leader_func = NULL,
        .movement_func = NULL,
        .arg = 0,
        .need_read = false,
    };
    
    rl_state->in_line.is_first_loop = true;

    if (rl_is_changing_mode_key(rl_state, line, c, shell_vars))
        return;
    
    while (true) {
        if (ctx.need_read){
            read(STDIN_FILENO, &ctx.c, 1);
            ctx.need_read = false;
        }

        switch (ctx.state) {
            case 0: {
                if (rl_isLeaderKey(ctx.c, &ctx.leader_func)) {
                    ctx.need_read = true;
                    ctx.state = 1;
                    continue;
                } else if (rl_isNumberKey(ctx.c, &ctx.arg)) {
                    ctx.state = 1;
                    ctx.need_read = true;
                    rl_state->in_line.arg = ctx.arg;

                    update_line(rl_state, line);
                    set_cursor_position(rl_state);
                    continue;
                } else if (rl_isMoveKey(ctx.c, &ctx.movement_func)){
                    ctx.state = 2;
                    continue;
                } else {
                    rl_state->in_line.arg = 0;
                    return;
                }
                break;
            }
            case 1: {
                if (rl_isNumberKey(ctx.c, &ctx.arg)) {

                    ctx.need_read = true;
                    rl_state->in_line.arg = ctx.arg;

                    update_line(rl_state, line);
                    set_cursor_position(rl_state);
                    continue;

                } else if (rl_isMoveKey(ctx.c, &ctx.movement_func)) {
                    ctx.state = 2;
                    continue;
                } else {
                    rl_state->in_line.arg = 0;
                    return;
                }
                break;
            }
            case 2: {
                rl_save_undo_state(line, rl_state);
                int cursor = rl_get_cursor_pos_on_line(rl_state);
                if (ctx.movement_func){
                    ctx.arg = (ctx.arg) ? ctx.arg : 1;
                    for (int i = 0; i < ctx.arg; i++){
                        ctx.movement_func(rl_state, line);
                        rl_state->in_line.is_first_loop = false;
                    }
                }

                if (ctx.leader_func){
                    if (leader_pressed_twice){
                        leader_pressed_twice = false;
                        cursor = 0;
                    }

                    rl_copy_from_n_to_cursor(rl_state, line, cursor);
                    if (ctx.leader_func)
                        ctx.leader_func(rl_state, line, cursor);
                }
                rl_state->in_line.arg = 0;
                return;
            }
            default: {return;};
        }
    }
}

int handle_printable_keys(readline_state_t *rl_state, char c, string *line, Vars *shell_vars){
    // rl_save_undo_state(line, rl_state);
	int pos = rl_state->cursor.y * get_col() - rl_state->current_prompt_size;
	pos += rl_state->cursor.x + ((rl_state->cursor.y == 0) ? rl_state->current_prompt_size : 0);

	if (c == '\n' || c == '\0'){
		return handle_enter_key(rl_state, line);
	}

    if (rl_state->in_line.mode == RL_VI && rl_state->in_line.vi_mode == VI_NORMAL){
        handle_vi_control(rl_state, c, line, RL_ALLOW_ALL, shell_vars);
        if (rl_state->in_line.exec_line){
            rl_state->in_line.exec_line = false;
            return RL_REFRESH;
        }
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
        // if (rl_state->in_line.vi_mode == VI_REPLACE && (size_t)pos < line->size)
        //     line->data[pos] = c;
        // else
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

rl_event handle_readline_controls(readline_state_t *rl_state, char c, string *line, Vars *shell_vars){
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
            case '\t':
                rl_autocomplete(rl_state, line, shell_vars); break;
            default: {
                da_pop(rl_state->undo_stack);
                return RL_NO_OP;
            }
        }
    }
    return RL_NO_OP;
}

void rl_switch_to_normal_mode(readline_state_t *rl_state){
    rl_state->in_line.vi_mode = VI_NORMAL;
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
            rl_switch_to_normal_mode(rl_state);
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
            rl_move_to_previous_word_start_alnum(rl_state, line); 
        } else if (seq[0] == 'f') {
            rl_move_to_next_word_start_alnum(rl_state, line);
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
