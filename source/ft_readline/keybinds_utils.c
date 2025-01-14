/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keybinds_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 11:16:12 by nbardavi          #+#    #+#             */
/*   Updated: 2025/01/14 16:18:26 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_readline.h"
#include "ft_regex.h"
#include "c_string.h"
#include "lexer.h"
#include "libft.h"
#include "utils.h"
#include "dynamic_arrays.h"

// ── Character access ────────────────────────────────────────────────
//
int rl_get_cursor_pos_on_line(readline_state_t *rl_state){
    return (rl_state->cursor.x * (rl_state->cursor.y + 1));
}

char rl_get_current_char(readline_state_t *rl_state, string *line){
    return (line->data[rl_state->cursor.x * (rl_state->cursor.y + 1)]);
}

char rl_get_next_char(readline_state_t *rl_state, string *line){
    size_t pos = (rl_state->cursor.x * (rl_state->cursor.y + 1)) + 1;
    if (pos > line->size) return '\03';
    return (line->data[pos]);
}

char rl_get_prev_char(readline_state_t *rl_state, string *line){
    int pos = (rl_state->cursor.x * (rl_state->cursor.y + 1)) - 1;
    if (pos < 0) return '\02';
    return (line->data[pos]);
}

/**
    * @brief cursor position relative
*/
char rl_get_n_char(readline_state_t *rl_state, string *line, int n){
    int pos = (rl_state->cursor.x * (rl_state->cursor.y + 1)) + n;
    if (pos < 0) return '\02';
    if (pos > (int)(line->size)) return '\03';
    return (line->data[pos]);
}

/**
    * @brief cursor position relative
*/
void rl_change_n_char(readline_state_t *rl_state, string *line, char c, int n){
    int pos = (rl_state->cursor.x * (rl_state->cursor.y + 1)) + n;
    if (pos < 0) return;
    if (pos > (int)(line->size)) return;
    line->data[pos] = c;
}

void rl_change_next_char(readline_state_t *rl_state, string *line, char c){
    size_t pos = (rl_state->cursor.x * (rl_state->cursor.y + 1)) + 1;
    if (pos > line->size) return;
    line->data[pos] = c;
}

void rl_change_prev_char(readline_state_t *rl_state, string *line, char c){
    int pos = (rl_state->cursor.x * (rl_state->cursor.y + 1)) - 1;
    if (pos < 0) return;
    line->data[pos] = c;
}

void rl_change_current_char(readline_state_t *rl_state, string *line, char c){
    line->data[rl_state->cursor.x * (rl_state->cursor.y + 1)] = c;
}
// ──────────────────────────────────────────────────────────────────────

// ── cursor_movement ─────────────────────────────────────────────────

int can_go_right(readline_state_t *rl_state, string *line) {
    int cols = get_col(); //number collums
    int tchars = line->size + rl_state->prompt_size;  //total chars
    int nlines = tchars / cols; //number lines
    int nchar_on_last_line = tchars % cols; 

    if (rl_state->cursor.y == 0) {
        if (nlines >= 1) {
            return rl_state->cursor.x < (int)(cols - rl_state->prompt_size);
        } else {
            return rl_state->cursor.x < (int)(nchar_on_last_line - rl_state->prompt_size);
        }
    }

    if (rl_state->cursor.y == nlines) {
        return rl_state->cursor.x < nchar_on_last_line;
    } 

    return rl_state->cursor.x < cols;
}

int can_go_left(readline_state_t *rl_state){
	if (rl_state->cursor.y == 0)
		return rl_state->cursor.x > 0;
	else
		return rl_state->cursor.x > -(int)rl_state->prompt_size;
}

// void rl_go_down(readline_state_t *rl_state, string *line){
//     size_t cols = get_col();
//     size_t cursor = rl_get_cursor_pos_on_line(rl_state);
//     
//     size_t goal = cols * (rl_state->cursor.y + 1);
//
//     if (goal < line->size){
//         if ((cursor + cols) < line->size){
//             rl_state->cursor.y++;
//         } else {
//             update_cursor_x(rl_state, line, line->size - cursor); // go to end
//         }
//     }
// }
//
// void rl_(readline_state_t *rl_state){
//     if (rl_state->cursor.y > 0){
//         rl_state->cursor.y--;
//     }
// }

void rl_move_back_by_char(readline_state_t *rl_state, string *line){
	if (can_go_left(rl_state)){
		update_cursor_x(rl_state, line, -1);
		set_cursor_position(rl_state);
	}
}

void rl_move_forward_by_char(readline_state_t *rl_state, string *line){
	if (can_go_right(rl_state, line)) {
		update_cursor_x(rl_state, line, 1);
		set_cursor_position(rl_state);
	}
}

void rl_move_to_end(readline_state_t *rl_state, string *line){
    while(can_go_right(rl_state, line)){
        rl_move_forward_by_char(rl_state, line);
    }
}

void rl_move_to_start(readline_state_t *rl_state, string *line){
    while(can_go_left(rl_state)){
        rl_move_back_by_char(rl_state, line);
    }
}

void rl_move_to_next_word_start(readline_state_t *rl_state, string *line, int (*compare_func)(int)) {
    while (can_go_right(rl_state, line) && !compare_func(rl_get_current_char(rl_state, line))){
        rl_move_forward_by_char(rl_state, line);
    }
    char next_char = rl_get_current_char(rl_state, line);
    while (can_go_right(rl_state, line) && compare_func(next_char) && next_char != '\0') {
        rl_move_forward_by_char(rl_state, line);
        next_char = rl_get_current_char(rl_state, line);
    } 
    while (can_go_right(rl_state, line) && !compare_func(rl_get_current_char(rl_state, line))){
        rl_move_forward_by_char(rl_state, line);
    }
}

void rl_move_to_previous_word_start(readline_state_t *rl_state, string *line, int (*compare_func)(int)) {
    while (can_go_left(rl_state) && !compare_func(rl_get_prev_char(rl_state, line))) {
        rl_move_back_by_char(rl_state, line);
    }

    char prev_char = rl_get_prev_char(rl_state, line);
    while (can_go_left(rl_state) && compare_func(prev_char) && prev_char != '\0') {
        rl_move_back_by_char(rl_state, line);
        prev_char = rl_get_prev_char(rl_state, line);
    }
}
// ──────────────────────────────────────────────────────────────────────

// ── history operation ───────────────────────────────────────────────
rl_event up_history(readline_state_t *rl_state, string *line) {
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
// ── string operation ────────────────────────────────────────────────

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

typedef struct s_word_pos {
    int curr_word_end;
    int curr_word_start;
    int prev_word_end;
    int prev_word_start;
} word_pos_t;

static bool is_alnum(const char c){
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'));
}

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

    rl_state->cursor.x = 0;
    rl_state->cursor.y = 0;
    update_cursor_x(rl_state, line, pos.curr_word_end + 1);
}
// ──────────────────────────────────────────────────────────────────────
