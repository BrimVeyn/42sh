/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_keys.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 11:37:52 by bvan-pae          #+#    #+#             */
/*   Updated: 2025/01/20 16:29:19 by nbardavi         ###   ########.fr       */
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

typedef void (rl_leader_func)(readline_state_t *, string *, size_t);
typedef void (rl_movement_func)(readline_state_t *, string *);

void rl_enter_leader_control(readline_state_t *rl_state, string *line, rl_leader_func *leader_func){
    char c = 0;
    int cursor = rl_get_cursor_pos_on_line(rl_state);

    do {
        read(STDIN_FILENO, &c, 1);
        handle_vi_control(rl_state, c, line, RL_ALLOW_LOW);
        
        update_line(rl_state, line);
        set_cursor_position(rl_state);
    } while(!had_a_movement);  //rl_get_cursor_pos_on_line(rl_state) == cursor);
    if (leader_pressed_twice){ //apply leader_func on whole line
        leader_pressed_twice = false;
        cursor = 0;
    }
    rl_copy_from_n_to_cursor(rl_state, line, cursor);
    if (leader_func)
        leader_func(rl_state, line, cursor);
}

// void handle_vi_controleee(readline_state_t *rl_state, char c, string *line, rl_vi_controls_mode mode){
//     
//     int args = rl_manage_args(RL_GET, 0);
//     if ((c >= '1' && c <= '9') || (args && c == '0')){
//         args = rl_manage_args(RL_SET, c - '0');
//         return;
//     }
//     args = (!args) ? 1 : args;
//
//     if (mode == RL_ALLOW_ALL){
//         switch (c){
//             case 'c':
//                 rl_enter_leader_control(rl_state, line, &rl_delete_from_n_to_cursor); switch_to_insert_mode(rl_state); break;
//             case 'd':
//                 rl_enter_leader_control(rl_state, line, &rl_delete_from_n_to_cursor); break;
//             case 'y':
//                 rl_enter_leader_control(rl_state, line , NULL); break; //NULL bc leader_control copy by default
//             default:
//                 break;
//         }
//     }
//
//     switch (c){
//         case 'j':
//             rl_repeat_by_args(rl_state, line, &down_history, args) ; break;
//         case 'k':
//             rl_repeat_by_args(rl_state, line, &up_history, args); break;
//         case 'i':
//             switch_to_insert_mode(rl_state); break;
//         case 'I':
//             switch_to_insert_mode(rl_state); rl_move_to_start(rl_state, line); break;
//         case 'a':
//             switch_to_insert_mode(rl_state); break;
//         case 'A':
//             switch_to_insert_mode(rl_state); rl_move_to_end(rl_state, line); break;
//         case 'l':
//             rl_repeat_by_args(rl_state, line, &rl_move_forward_by_char, args); break;
//         case 'h':
//             rl_repeat_by_args(rl_state, line, &rl_move_back_by_char, args); break;
//         case 'w':
//             rl_repeat_by_args_with_comp(rl_state, line, &ft_isalnum, &rl_move_to_next_word_start, args); break;
//         case 'W':
//             rl_repeat_by_args_with_comp(rl_state, line, &ft_isnotspace, &rl_move_to_next_word_start, args); break;
//         case 'b':
//             rl_repeat_by_args_with_comp(rl_state, line, &ft_isalnum, &rl_move_to_previous_word_start, args); break;
//         case 'B':
//             rl_repeat_by_args_with_comp(rl_state, line, &ft_isnotspace, &rl_move_to_previous_word_start, args); break;
//         case '0':
//             rl_move_to_start(rl_state, line);break;
//         case '$':
//             rl_move_to_end(rl_state, line);break;
//         case '^':
//             rl_move_to_first_char(rl_state, line);break;
//         case 'e':
//             rl_repeat_by_args_with_comp(rl_state, line, &ft_isalnum, &rl_move_to_next_word_end, args); break;
//         case 'E':
//             rl_repeat_by_args_with_comp(rl_state, line, &ft_isnotspace, &rl_move_to_next_word_end, args); break;
//         case '|':
//             rl_move_to_n_index(rl_state, line, args); break;
//         case 'f':
//             rl_move_to_next_matching_char(rl_state, line, args, RL_NEWMATCH); break;
//         case 'F':
//             rl_move_to_prev_matching_char(rl_state, line, args, RL_NEWMATCH); break;
//         case ';':
//             rl_manage_matching_vi_mode(NULL, RL_GET)(rl_state, line, args, RL_REMATCH); break;
//         case 's':
//             rl_repeat_by_args(rl_state, line, rl_substitute_current_char, args); break;
//         case 'S':
//             rl_substitute_line(rl_state, line); break;
//         case 'c':
//             rl_move_to_end(rl_state, line); leader_pressed_twice = true; break;
//         case 'C':
//             rl_delete_until_end(rl_state, line); switch_to_insert_mode(rl_state); break;
//         case 'd':
//             rl_move_to_end(rl_state, line); leader_pressed_twice = true; break;
//         case 'D':
//             rl_delete_until_end(rl_state, line); break;
//         case 'x':
//             rl_repeat_by_args(rl_state, line, &rl_delete_curr_char, args); break;
//         case 'X':
//             rl_repeat_by_args(rl_state, line, &rl_delete_prev_char, args); break;
//         case 'p':
//             rl_repeat_by_args(rl_state, line, &rl_paste_after_cursor, args); break;
//         case 'P':
//             rl_repeat_by_args(rl_state, line, &rl_paste_on_cursor, args); break;
//         case 'y':
//             rl_move_to_end(rl_state, line); leader_pressed_twice = true; break;
//         case 'Y':
//             rl_copy_until_end(rl_state, line); break;
//         default:
//             return;
//     }
//
//     rl_manage_args(RL_RESET, 0);
// }

//0: 11111111111111111111111111111111
//1: 00000010000000000000000000000000
//2: 

bool rl_isMoveKey(char c, rl_movement_func **func) {
    move_cursor(0, 0);
    // set_cursor_position(rl_state);
    switch (c) {
        case 'j': { *func = &down_history; return true; }
        case 'k': { *func = &up_history; return true; }
        case 'l': { *func = &rl_move_forward_by_char; return true; }
        case 'h': { *func = &rl_move_back_by_char; return true; }
        case 'w': { *func = &rl_move_to_next_word_start_alnum; return true; }
        case 'W': { *func = &rl_move_to_next_word_start_sp; return true; }
        case 'b': { *func = &rl_move_to_previous_word_start_sp; return true; }
        case 'B': { *func = &rl_move_to_previous_word_start_alnum; return true; }
        case 'e': { *func = &rl_move_to_next_word_end_alnum; return true; }
        case 'E': { *func = &rl_move_to_next_word_end_sp; return true; }
        case '0': { *func = &rl_move_to_start; return true; }
        case '$': { *func = &rl_move_to_end; return true; }
        case '^': { *func = &rl_move_to_first_char; return true; }
        // case '|': { func = &rl_move_to_n_index; return true; }
        // case 'f': { func = &rl_move_to_next_matching_char; return true; }
        // case 'F': { func = &rl_move_to_prev_matching_char; return true; }
        // case ';': { func = rl_manage_matching_vi_mode(NULL, RL_GET); return true; }
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

bool rl_isLeaderKey(char c, rl_leader_func **func) {
    switch (c) {
        case 'c': { *func = &rl_delete_from_n_to_cursor; return true; };
        case 'y': { *func = &rl_delete_from_n_to_cursor; return true; };
        case 'd': { *func = &rl_delete_from_n_to_cursor; return true; };
    }
    return false;
}

void handle_vi_control(readline_state_t *rl_state, char c, string *line, rl_vi_controls_mode mode){
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
                    continue;
                } else if (rl_isMoveKey(ctx.c, &ctx.movement_func)){
                    ctx.state = 2;
                    continue;
                } else {
                    return;
                }
                break;
                // } else if (isDirection()) { //f or t
                //     ctx.state = 3;
                //     continue;
                // } else if (isMove()) {
                //     ctx.state = 4;
                // }
            }
            case 1: {
                if (rl_isNumberKey(ctx.c, &ctx.arg)) {
                    ctx.need_read = true;
                    rl_state->in_line.arg = ctx.arg;
                    update_line(rl_state, line);
                    continue;
                } else if (rl_isMoveKey(ctx.c, &ctx.movement_func)) {
                    ctx.state = 2;
                    continue;
                } else {
                    // move_cursor(0, 0);
                    // ft_dprintf(2, "SALUT3\n");
                    // set_cursor_position(rl_state);
                    return;
                }
                // break;
            }
            case 2: {
                int cursor = rl_get_cursor_pos_on_line(rl_state);
                if (ctx.movement_func){
                    ctx.arg = (ctx.arg) ? ctx.arg : 1;
                    for (int i = 0; i < ctx.arg; i++){
                        ctx.movement_func(rl_state, line);
                    }
                } else {

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
                rl_manage_args(RL_RESET, 0);
                return;
            }
            default: {return;};
        }
    }
    rl_manage_args(RL_RESET, 0);
}

/*
 * commit: Added multiple shortcuts for vi mode: d, D, 
*/

int handle_printable_keys(readline_state_t *rl_state, char c, string *line){
    // rl_save_undo_state(line, rl_state);
	int pos = rl_state->cursor.y * get_col() - rl_state->current_prompt_size;
	pos += rl_state->cursor.x + ((rl_state->cursor.y == 0) ? rl_state->current_prompt_size : 0);

	if (c == '\n' || c == '\0'){
		return handle_enter_key(rl_state, line);
	}

    if (rl_state->in_line.mode == RL_VI && rl_state->in_line.vi_mode == VI_NORMAL){
        handle_vi_control(rl_state, c, line, RL_ALLOW_ALL);
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
