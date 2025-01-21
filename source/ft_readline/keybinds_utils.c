/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keybinds_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 11:16:12 by nbardavi          #+#    #+#             */
/*   Updated: 2025/01/21 15:44:19 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stdio.h>
#include <unistd.h>

#include "final_parser.h"
#include "ft_readline.h"
#include "ft_regex.h"
#include "c_string.h"
#include "lexer.h"
#include "libft.h"
#include "utils.h"
#include "dynamic_arrays.h"

void rl_repeat_by_args(readline_state_t *rl_state, string *line, void (*command_func)(readline_state_t *, string *), size_t n){
    for (size_t i = 0; i < n; i++){
        command_func(rl_state, line);
    }
}

void rl_repeat_by_args_with_comp(readline_state_t *rl_state, string *line, int (*compare_func) (int), void (*command_func)(readline_state_t *, string *, int (*compare_func)(int)), size_t n){
    for (size_t i = 0; i < n; i++){
        command_func(rl_state, line, compare_func);
    }
}

/**
    * @brief copy need to be allocated and added to gc
*/
char *rl_manage_clipboard(manage_rl_accessor mode, char *copy){
    static char *copied = NULL;
    
    if (mode == RL_SET){
        if (copied){
            gc(GC_FREE, copied, GC_READLINE);
        }
        copied = copy;
    }
    return copied;
}

void rl_handle_find_next_key(readline_state_t *rl_state, string *line){
    rl_move_to_next_matching_char(rl_state, line, RL_NEWMATCH);
}

void rl_handle_find_prev_key(readline_state_t *rl_state, string *line){
    rl_move_to_prev_matching_char(rl_state, line, RL_NEWMATCH);
}

void rl_handle_redo_previous_match(readline_state_t *rl_state, string *line){
    rl_manage_matching_vi_mode(NULL, RL_GET)(rl_state, line);
}

void rl_handle_redo_next(readline_state_t *rl_state, string *line){
    rl_move_to_next_matching_char(rl_state, line, RL_REMATCH);
}

void rl_handle_redo_prev(readline_state_t *rl_state, string *line){
    rl_move_to_prev_matching_char(rl_state, line, RL_REMATCH);
}

// ── Character access ────────────────────────────────────────────────

void rl_delete_prev_char(readline_state_t *rl_state, string *line) {
    size_t cursor = rl_get_cursor_pos_on_line(rl_state);
    if (cursor == 0 || line->size == 0)
        return;

    cursor--;
    ft_memmove(line->data + cursor, line->data + cursor + 1, line->size - cursor - 1);
    line->size--;
    line->data[line->size] = '\0';
    update_cursor_x(rl_state, line, -1);
}

void rl_delete_curr_char(readline_state_t *rl_state, string *line) {
    size_t cursor = rl_get_cursor_pos_on_line(rl_state);
    if (cursor >= line->size || line->size == 0)
        return;

    ft_memmove(line->data + cursor, line->data + cursor + 1, line->size - cursor - 1);
    line->size--;
    line->data[line->size] = '\0';
}

int rl_get_cursor_pos_on_line(readline_state_t *rl_state) {
    if (rl_state->cursor.y == 0){
        return rl_state->cursor.x;
    } else {
        return rl_state->cursor.x + (rl_state->cursor.y * get_col()) - rl_state->current_prompt_size;
    }
}

char rl_get_current_char(readline_state_t *rl_state, string *line) {
    int pos = rl_get_cursor_pos_on_line(rl_state);
    if (pos >= 0 && pos < (int)line->size)
        return line->data[pos];
    return '\0';
}

char rl_get_next_char(readline_state_t *rl_state, string *line) {
    size_t pos = rl_get_cursor_pos_on_line(rl_state) + 1;
    if (pos >= line->size)
        return '\03';
    return line->data[pos];
}

char rl_get_prev_char(readline_state_t *rl_state, string *line) {
    int pos = rl_get_cursor_pos_on_line(rl_state) - 1;
    if (pos < 0)
        return '\02';
    return line->data[pos];
}

char rl_get_n_char(readline_state_t *rl_state, string *line, int n) {
    int pos = rl_get_cursor_pos_on_line(rl_state) + n;
    if (pos < 0 || pos >= (int)line->size)
        return pos < 0 ? '\02' : '\03';
    return line->data[pos];
}

void rl_change_n_char(readline_state_t *rl_state, string *line, char c, int n) {
    int pos = rl_get_cursor_pos_on_line(rl_state) + n;
    if (pos < 0 || pos >= (int)line->size)
        return;
    line->data[pos] = c;
}

void rl_change_next_char(readline_state_t *rl_state, string *line, char c) {
    size_t pos = rl_get_cursor_pos_on_line(rl_state) + 1;
    if (pos >= line->size)
        return;
    line->data[pos] = c;
}

void rl_change_prev_char(readline_state_t *rl_state, string *line, char c) {
    int pos = rl_get_cursor_pos_on_line(rl_state) - 1;
    if (pos < 0)
        return;
    line->data[pos] = c;
}

void rl_change_current_char(readline_state_t *rl_state, string *line, char c) {
    int pos = rl_get_cursor_pos_on_line(rl_state);
    if (pos >= 0 && pos < (int)line->size)
        line->data[pos] = c;
}
// ──────────────────────────────────────────────────────────────────────

// ── cursor_movement ─────────────────────────────────────────────────

void (*rl_manage_matching_vi_mode(
    void (*matching_func)(readline_state_t *, string *),
    manage_rl_accessor mode))(readline_state_t *, string *) 
{
    static void (*last_function)(readline_state_t *, string *) = NULL;

    if (mode == RL_SET) {
        last_function = matching_func;
    }
    return last_function;
}

void rl_move_to_next_matching_char(readline_state_t *rl_state, string *line, rl_matching_mode mode){
    static char c = 0;

    if (mode == RL_NEWMATCH){
        read(STDIN_FILENO, &c, 1);
    }

    if (c){
        rl_manage_matching_vi_mode(&rl_handle_redo_next, RL_SET);

        size_t cursor_pos = rl_get_cursor_pos_on_line(rl_state);
        for (size_t i = cursor_pos + 1; i < line->size; i++){
            if (c == line->data[i]){
                update_cursor_x(rl_state, line, i - cursor_pos);
                break;
            }
        }
    }
}

void rl_move_to_prev_matching_char(readline_state_t *rl_state, string *line, rl_matching_mode mode){
    static char c = 0;
    
    if (mode == RL_NEWMATCH){
        read(STDIN_FILENO, &c, 1);
    }

    if (c){
        rl_manage_matching_vi_mode(&rl_handle_redo_prev, RL_SET);

        size_t cursor_pos = rl_get_cursor_pos_on_line(rl_state);
        for (size_t i = cursor_pos - 1; i > 0; i--){
            if (c == line->data[i]){
                update_cursor_x(rl_state, line, i - cursor_pos);
                break;
            }
        }
    }
}

void rl_move_to_n_index(readline_state_t *rl_state, string *line){
    if (rl_state->in_line.is_first_loop){
        rl_move_to_start(rl_state, line);
        return;
    } else {
        rl_move_forward_by_char(rl_state, line);
    }
}

static bool ft_iswspace(int c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}

void rl_move_to_first_char(readline_state_t *rl_state, string *line){
    while(ft_iswspace((rl_get_current_char(rl_state, line)))){
        rl_move_forward_by_char(rl_state, line);
    }
}

int can_go_right(readline_state_t *rl_state, string *line) {
    int cols = get_col(); //number collums
    int tchars = line->size + rl_state->current_prompt_size;  //total chars
    int nlines = tchars / cols; //number lines
    int nchar_on_last_line = tchars % cols; 

    if (rl_state->cursor.y == 0) {
        if (nlines >= 1) {
            return rl_state->cursor.x < (int)(cols - rl_state->current_prompt_size);
        } else {
            return rl_state->cursor.x < (int)(nchar_on_last_line - rl_state->current_prompt_size);
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
		return rl_state->cursor.x > -(int)rl_state->current_prompt_size;
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
	}
}

void rl_move_forward_by_char(readline_state_t *rl_state, string *line){
	if (can_go_right(rl_state, line))
		update_cursor_x(rl_state, line, 1);
}

void rl_move_to_end(readline_state_t *rl_state, string *line){
    while(can_go_right(rl_state, line))
        rl_move_forward_by_char(rl_state, line);
}

void rl_move_to_start(readline_state_t *rl_state, string *line){
    while(can_go_left(rl_state)){
        rl_move_back_by_char(rl_state, line);
    }
}

static int ft_isnotspace(int c){
    return (c != ' ');
}

void rl_move_to_next_word_end_alnum(readline_state_t *rl_state, string *line){
    while (can_go_right(rl_state, line) && !ft_isalnum(rl_get_current_char(rl_state, line))){
        rl_move_forward_by_char(rl_state, line);
    }
    char next_char = rl_get_current_char(rl_state, line);
    while (can_go_right(rl_state, line) && ft_isalnum(next_char) && next_char != '\0') {
        rl_move_forward_by_char(rl_state, line);
        next_char = rl_get_current_char(rl_state, line);
    } 
}

void rl_move_to_next_word_end_sp(readline_state_t *rl_state, string *line){
    while (can_go_right(rl_state, line) && !ft_isnotspace(rl_get_current_char(rl_state, line))){
        rl_move_forward_by_char(rl_state, line);
    }
    char next_char = rl_get_current_char(rl_state, line);
    while (can_go_right(rl_state, line) && ft_isnotspace(next_char) && next_char != '\0') {
        rl_move_forward_by_char(rl_state, line);
        next_char = rl_get_current_char(rl_state, line);
    } 
}

void rl_move_to_previous_word_end_alnum(readline_state_t *rl_state, string *line) {
    while (can_go_left(rl_state) && !ft_isalnum(rl_get_prev_char(rl_state, line))) {
        rl_move_back_by_char(rl_state, line);
    }
}

void rl_move_to_previous_word_end_sp(readline_state_t *rl_state, string *line) {
    while (can_go_left(rl_state) && !ft_isnotspace(rl_get_prev_char(rl_state, line))) {
        rl_move_back_by_char(rl_state, line);
    }
}

void rl_move_to_next_word_start_sp(readline_state_t *rl_state, string *line) {
    while (can_go_right(rl_state, line) && !ft_isnotspace(rl_get_current_char(rl_state, line))){
        rl_move_forward_by_char(rl_state, line);
    }
    char next_char = rl_get_current_char(rl_state, line);
    while (can_go_right(rl_state, line) && ft_isnotspace(next_char) && next_char != '\0') {
        rl_move_forward_by_char(rl_state, line);
        next_char = rl_get_current_char(rl_state, line);
    } 
    while (can_go_right(rl_state, line) && !ft_isnotspace(rl_get_current_char(rl_state, line))){
        rl_move_forward_by_char(rl_state, line);
    }
}

void rl_move_to_next_word_start_alnum(readline_state_t *rl_state, string *line) {
    while (can_go_right(rl_state, line) && !ft_isalnum(rl_get_current_char(rl_state, line))){
        rl_move_forward_by_char(rl_state, line);
    }
    char next_char = rl_get_current_char(rl_state, line);
    while (can_go_right(rl_state, line) && ft_isalnum(next_char) && next_char != '\0') {
        rl_move_forward_by_char(rl_state, line);
        next_char = rl_get_current_char(rl_state, line);
    } 
    while (can_go_right(rl_state, line) && !ft_isalnum(rl_get_current_char(rl_state, line))){
        rl_move_forward_by_char(rl_state, line);
    }
}

void rl_move_to_previous_word_start_sp(readline_state_t *rl_state, string *line) {
    while (can_go_left(rl_state) && !ft_isnotspace(rl_get_prev_char(rl_state, line))) {
        rl_move_back_by_char(rl_state, line);
    }

    char prev_char = rl_get_prev_char(rl_state, line);
    while (can_go_left(rl_state) && ft_isnotspace(prev_char) && prev_char != '\0') {
        rl_move_back_by_char(rl_state, line);
        prev_char = rl_get_prev_char(rl_state, line);
    }
}

void rl_move_to_previous_word_start_alnum(readline_state_t *rl_state, string *line){
    while (can_go_left(rl_state) && !ft_isalnum(rl_get_prev_char(rl_state, line))) {
        rl_move_back_by_char(rl_state, line);
    }

    char prev_char = rl_get_prev_char(rl_state, line);
    while (can_go_left(rl_state) && ft_isalnum(prev_char) && prev_char != '\0') {
        rl_move_back_by_char(rl_state, line);
        prev_char = rl_get_prev_char(rl_state, line);
    }
}


// ──────────────────────────────────────────────────────────────────────
// ── clipboard operation ─────────────────────────────────────────────
void rl_copy_until_end(readline_state_t *rl_state, string *line){
    if (line->size == 0)
        return;

    size_t cursor = rl_get_cursor_pos_on_line(rl_state);
    char *copy = ft_substr(line->data, cursor, line->size - cursor);
    gc(GC_ADD, copy, GC_READLINE);
    rl_manage_clipboard(RL_SET, copy);
}

void rl_copy_from_n_to_cursor(readline_state_t *rl_state, string *line, size_t n){
    if (line->size == 0 || n >= line->size) {
        return;
    }

    size_t cursor = rl_get_cursor_pos_on_line(rl_state);

    size_t pos1 = n < cursor ? n : cursor;
    size_t pos2 = n > cursor ? n : cursor;
    size_t len = pos2 - pos1;
    
    char *copy = ft_substr(line->data, pos1, len);
    gc(GC_ADD, copy, GC_READLINE);
    rl_manage_clipboard(RL_SET, copy);
}

// ──────────────────────────────────────────────────────────────────────
// ── history operation ───────────────────────────────────────────────
void up_history(readline_state_t *rl_state, string *line) {
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
}

void down_history(readline_state_t *rl_state, string *line) {
	if (history->navigation_offset > 0){
		history->navigation_offset--;
		rl_state->cursor.x = history->entries[history->length - history->navigation_offset - 1]->line.size;
		gc(GC_FREE, line->data, GC_READLINE);
		*line = str_strdup(&history->entries[history->length - history->navigation_offset - 1]->line);
		gc(GC_ADD, line->data, GC_READLINE);
	}
}
// ── string operation ────────────────────────────────────────────────

void rl_replace_current_char(readline_state_t *rl_state, string *line){
    static char c = '\0';

    if (!rl_state->in_line.is_first_loop){
        rl_move_forward_by_char(rl_state, line);
    }
    if (rl_state->in_line.is_first_loop)
        read(STDIN_FILENO, &c, 1);
    if (line->size && rl_get_current_char(rl_state, line) != '\0' && c){
        rl_change_current_char(rl_state, line, c);
    }
}

void rl_paste_after_cursor(readline_state_t *rl_state, string *line){
    char *copied = rl_manage_clipboard(RL_GET, NULL);
    if (!copied) return;
    
    rl_move_forward_by_char(rl_state, line);
    size_t cursor = rl_get_cursor_pos_on_line(rl_state);
    str_insert_str(line, copied, cursor);
    update_cursor_x(rl_state, line, ft_strlen(copied));
}

void rl_paste_on_cursor(readline_state_t *rl_state, string *line){
    char *copied = rl_manage_clipboard(RL_GET, NULL);
    if (!copied) return;
    
    size_t cursor = rl_get_cursor_pos_on_line(rl_state);
    str_insert_str(line, copied, cursor);
    update_cursor_x(rl_state, line, ft_strlen(copied));
}

void rl_clear_line (string *line){
    if (!line->size) return;
    ft_memset(line->data, 0, line->size);
    line->size = 0;
}

void rl_substitute_line (readline_state_t *rl_state, string *line){
    if (!line->size) return;
    rl_clear_line(line);
    rl_state->cursor.x = 0;
    rl_state->cursor.y = 0;
    switch_to_insert_mode(rl_state);
}

void rl_substitute_current_char(readline_state_t *rl_state, string *line){
    rl_delete_curr_char(rl_state, line);
    switch_to_insert_mode(rl_state);
}
              
void rl_change_in_word(readline_state_t *rl_state, string *line) {
    if (line->size == 0) return;
    int cursor = rl_get_cursor_pos_on_line(rl_state);
    size_t word_end = cursor + 1;

    for (; word_end < line->size && ft_isalnum(line->data[word_end]); word_end++) {}

    ft_memmove(line->data + cursor, line->data + word_end, line->size - word_end);
    ft_memset(line->data + (line->size - (word_end - cursor)), 0, word_end - cursor);

    line->size -= (word_end - cursor);

    switch_to_insert_mode(rl_state);
}

void rl_delete_until_end(readline_state_t *rl_state, string *line){
    if (!line->size) return;
    size_t cursor = rl_get_cursor_pos_on_line(rl_state);
    ft_memset(line->data + cursor, 0, line->size - cursor);
    line->size = cursor;
}

void rl_delete_from_n_to_cursor(readline_state_t *rl_state, string *line, size_t n) {
    if (line->size == 0 || n >= line->size) {
        return;
    }

    size_t cursor = rl_get_cursor_pos_on_line(rl_state);

    size_t pos1 = n < cursor ? n : cursor;
    size_t pos2 = n > cursor ? n : cursor;

    size_t delete_count = pos2 - pos1;

    ft_memmove(line->data + pos1, line->data + pos2, line->size - pos2);

    ft_memset(line->data + (line->size - delete_count), 0, delete_count);

    line->size -= delete_count;
    rl_state->cursor.x = 0;
    rl_state->cursor.y = 0;
    update_cursor_x(rl_state, line, pos1);
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
