/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_keys.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 16:26:41 by nbardavi          #+#    #+#             */
/*   Updated: 2024/11/04 15:10:39 by nbardavi         ###   ########.fr       */
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


//TODO: Clean handle_enter_function
int handle_enter_key(readline_state_t *rl_state, string *line) {
	if (rl_state->interactive){
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

void handle_backspace_key(readline_state_t *rl_state, string *line, size_t pos){
	if (!pos)
		return;
	str_erase(line, pos - 1, 1);
	update_cursor_x(rl_state, line, -1);
}


int handle_printable_keys(readline_state_t *rl_state, char c, string *line){
	
	int pos = rl_state->cursor.y * get_col() - rl_state->prompt.size;
	pos += rl_state->cursor.x + ((rl_state->cursor.y == 0) ? rl_state->prompt.size : 0);

	if ((c == '\n' && rl_state->interactive) || (c == '\0' && !rl_state->interactive)){
		return handle_enter_key(rl_state, line);
	}

	if (c == 127 && rl_state->interactive){
		handle_backspace_key(rl_state, line, pos);
		return 0;
	}
	
	str_insert(line, c, pos);

	if (rl_state->interactive)
		update_cursor_x(rl_state, line, 1);

    return 0;
}

void handle_delete_key(readline_state_t *rl_state, string *line) {
    int pos = rl_state->cursor.y * get_col() - rl_state->prompt.size;
    pos += rl_state->cursor.x + ((rl_state->cursor.y == 0) ? rl_state->prompt.size : 0);

    if (pos >= str_length(line)) return;

    str_erase(line, pos, 1);

    if (rl_state->interactive) {
        set_cursor_position(rl_state);
    }
}

void handle_left_arrow(readline_state_t *rl_state, string *line){
	if (can_go_left(rl_state)){
		update_cursor_x(rl_state, line, -1);
		set_cursor_position(rl_state);
	}
}

void handle_right_arrow(readline_state_t *rl_state, string *line){
	if (can_go_right(rl_state, line)) {
		update_cursor_x(rl_state, line, 1);
		set_cursor_position(rl_state);
	}
}

int handle_down_arrow(readline_state_t *rl_state, string *line) {
	history->offset++;
	rl_state->cursor.x = history->entries[history->offset]->line.size;
	gc(GC_FREE, line->data, GC_READLINE);
	*line = str_strdup(&history->entries[history->offset]->line);
	gc(GC_ADD, line->data, GC_READLINE);
	return 2;
}

int handle_up_arrow(readline_state_t *rl_state, string *line) {

	history->offset--;
	rl_state->cursor.x = history->entries[history->offset]->line.size;
	gc(GC_FREE, line->data, GC_READLINE);
	*line = str_strdup(&history->entries[history->offset]->line);
	gc(GC_ADD, line->data, GC_READLINE);
	return 2;
}

int handle_special_keys(readline_state_t *rl_state, string *line) {
    char seq[3];
    if (read(STDIN_FILENO, &seq[0], 1) == 0) return 1;
    
    if (seq[0] == '[') {
        if (read(STDIN_FILENO, &seq[1], 1) == 0) return 1;

        if (seq[1] == '3') {
            if (read(STDIN_FILENO, &seq[2], 1) == 0) return 1;
            if (seq[2] == '~') {
                handle_delete_key(rl_state, line);
                return 2;
            }
        }

		//leave search mode if special key other than DEL is pressed
        if (rl_state->search_mode.active) {
            rl_state->search_mode.active = false;
            rl_state->prompt.size = ft_strlen(rl_state->prompt.data);
        }
        
        if (seq[1] == 'A' && history->offset > 0) {
            return handle_up_arrow(rl_state, line);
        } else if (seq[1] == 'B' && history->offset < history->length - 1) {
            return handle_down_arrow(rl_state, line);
        } else if (seq[1] == 'D') {
			handle_left_arrow(rl_state, line);
        } else if (seq[1] == 'C'){
			handle_right_arrow(rl_state, line);
        }
    }
    return 0;
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
