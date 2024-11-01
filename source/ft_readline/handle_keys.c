/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_keys.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 16:26:41 by nbardavi          #+#    #+#             */
/*   Updated: 2024/11/01 16:33:09 by nbardavi         ###   ########.fr       */
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

//TODO:separer \n backspace car pas logique
int handle_printable_keys(readline_state_t *rl_state, char c, string *line){
	
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

void handle_delete_key(readline_state_t *rl_state, string *line) {
    int pos = rl_state->cursor.y * get_col() - rl_state->prompt.size;
    pos += rl_state->cursor.x + ((rl_state->cursor.y == 0) ? rl_state->prompt.size : 0);

    if (pos >= str_length(line)) return;

    str_erase(line, pos, 1);

    if (rl_state->interactive) {
        set_cursor_position(rl_state);
    }
}

int handle_special_keys(readline_state_t *rl_state, string *line) {
    char seq[3];
    if (read(STDIN_FILENO, &seq[0], 1) == 0) return 1;
    
    if (seq[0] == '[') {
        if (read(STDIN_FILENO, &seq[1], 1) == 0) return 1;

        if (seq[1] == '3') { // Cas de DEL
            if (read(STDIN_FILENO, &seq[2], 1) == 0) return 1;
            if (seq[2] == '~') {
                handle_delete_key(rl_state, line);
                return 2;
            }
        }

        if (rl_state->search_mode.active) {
            rl_state->search_mode.active = false;
            rl_state->prompt.size = ft_strlen(rl_state->prompt.data);
        }
        
        if (seq[1] == 'A' && history->offset > 0) { // Flèche haut
            history->offset--;
            rl_state->cursor.x = history->entries[history->offset]->line.size;
            gc(GC_FREE, line->data, GC_READLINE);
            *line = str_strdup(&history->entries[history->offset]->line);
            gc(GC_ADD, line->data, GC_READLINE);
            return 2;
        } else if (seq[1] == 'B' && history->offset < history->length - 1) { // Flèche bas
            history->offset++;
            rl_state->cursor.x = history->entries[history->offset]->line.size;
            gc(GC_FREE, line->data, GC_READLINE);
            *line = str_strdup(&history->entries[history->offset]->line);
            gc(GC_ADD, line->data, GC_READLINE);
            return 2;
        } else if (seq[1] == 'D' && can_go_left(rl_state)) { // Flèche gauche
            update_cursor_x(rl_state, line, -1);
            set_cursor_position(rl_state);
        } else if (seq[1] == 'C' && can_go_right(rl_state, line)) { // Flèche droite
            update_cursor_x(rl_state, line, 1);
            set_cursor_position(rl_state);
        }
    }
    return 0;
}

void handle_control_keys(readline_state_t *rl_state, char char_c){
	if (char_c == 12){
		write(STDOUT_FILENO, "\033[2J", 4);
		rl_state->cursor.y = 0;
		rl_state->cursor_offset.y = 0;
		rl_state->cursor_offset.x = 0;
		move_cursor(0, 0);
		rl_print_prompt(STDOUT_FILENO, rl_state);
	}
	if (char_c == 18){
		rl_state->search_mode.active = true;
	}
}
