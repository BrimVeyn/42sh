/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_readline.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:15:30 by nbardavi          #+#    #+#             */
/*   Updated: 2025/01/23 13:29:21 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_READLINE_H
# define FT_READLINE_H


#ifndef C_STRING_H
# include "c_string.h"
#endif

#include <stdbool.h>

#define CTRL_L 12 
#define CTRL_R 18
#define CURRENT_LINE 1

typedef struct _hist_entry {
  string line;
} HIST_ENTRY;

typedef struct _hist_config {
	int histsize;
	int histfilesize;
} HISTORY_CONFIG;

typedef struct _hist_state {
	HIST_ENTRY **entries; /* Pointer to the entries themselves. */
	int offset;           /* The location pointer within this array. */
	int navigation_offset;
	int length;           /* Number of elements within this array. */
	int capacity;             /* Number of slots allocated to this array. */
    HISTORY_CONFIG *config;
} HISTORY_STATE;

typedef struct s_position{
    int x;
    int y;
} position_t;

typedef struct s_undo_state {
    string line;
    position_t cursor;
}   undo_state_t;

typedef struct s_undo_state_stack {
    undo_state_t **data;
	size_t	size;
	size_t	capacity;
	size_t	size_of_element;
	int		gc_level;
}   undo_state_stack_t;

typedef enum {
	PRINT_KEY,
    DELETE_KEY,
} rl_last_action;

typedef enum {
	RL_GET,
	RL_SET,
    RL_RESET,
} manage_rl_accessor;

typedef enum {
	RL_NO_OP,
	RL_REFRESH,
} rl_event;

typedef enum {
    VI_INSERT,
    VI_NORMAL,
    VI_REPLACE,
} rl_vi_mode;

typedef enum {
    RL_VI,
    RL_READLINE,
} rl_inline_mode;

typedef enum {
    RL_NEWMATCH,
    RL_REMATCH,
    RL_REMATCH_REVERSE,
} rl_matching_mode;

typedef enum {
    RL_ALLOW_ALL,
    RL_ALLOW_LOW,
}   rl_vi_controls_mode;

typedef struct s_search_mode {
	char *word_found;
	int word_start;
	bool active;
} search_mode_t;

typedef struct s_inline {
    rl_inline_mode mode;
    rl_vi_mode vi_mode;
    int arg;
    bool is_first_loop;
    bool exec_line;
} inline_t;

typedef struct s_suggestion {
    char *formated_string;
    bool active;
    char *word;
    enum { RL_S_FOUND, RL_S_NOTFOUND} mode;
} suggestion_t;

typedef struct s_readline_state {
	char *current_prompt;
    char *normal_prompt;
	size_t current_prompt_size;
	position_t cursor_offset;
	position_t cursor;
	search_mode_t search_mode;
    undo_state_stack_t *undo_stack;
	bool interactive;
    inline_t in_line;
    suggestion_t suggestion;
} readline_state_t;

typedef void (rl_leader_func)(readline_state_t *, string *, size_t);
typedef void (rl_movement_func)(readline_state_t *, string *);
typedef void (rl_matching_func)(readline_state_t *, string *, rl_matching_mode);

typedef struct s_vi_state{
    int state;
    char c;
    rl_leader_func *leader_func;
    rl_movement_func *movement_func;
    int arg;
    bool need_read;
} vi_state_t;

extern int rl_done;
extern HISTORY_STATE *history;
extern bool history_defined;

void destroy_history();
HIST_ENTRY *pop_history();
void str_info(const string *str);
void set_prompt(readline_state_t *rl_state, const char *new_prompt);
int search_in_history(readline_state_t *rl_state, char *str);

void handle_control_keys(readline_state_t *rl_state, char char_c);
int can_go_right(readline_state_t *rl_state, string *line);
int can_go_right_vi(readline_state_t *rl_state, string *line);
int can_go_left(readline_state_t *rl_state);

void ft_readline_clean();
void ft_rl_newline();
char rl_get_current_char(readline_state_t *rl_state, string *line);
char rl_get_prev_char(readline_state_t *rl_state, string *line);
char rl_get_next_char(readline_state_t *rl_state, string *line);


void update_line(readline_state_t *rl_state, string *line);
void update_cursor_x(readline_state_t *rl_state, string *line, ssize_t n);

size_t get_col(void);
size_t get_row(void);
void rl_print_prompt(int fd, readline_state_t *rl_state);

void move_cursor(int x, int y);

void set_cursor_position(readline_state_t *rl_state);
void print_history_values(HISTORY_STATE *history);

#include "final_parser.h"

rl_event handle_readline_controls(readline_state_t *rl_state, char c, string *line, Vars *shell_vars);
rl_event handle_special_keys(readline_state_t *rl_state, string *line);
char *ft_readline(const char *prompt, Vars *shell_vars);
void init_history(Vars *shell_vars);
void add_history(const char *str, Vars *shell_vars);
void handle_history_config(HISTORY_STATE *history, Vars *shell_vars);

char *rl_manage_clipboard(manage_rl_accessor mode, char *copy);
int rl_manage_args(manage_rl_accessor mode, int n);
void rl_repeat_by_args(readline_state_t *rl_state, string *line, void (*command_func)(readline_state_t *, string *), size_t n);
void rl_repeat_by_args_with_comp(readline_state_t *rl_state, string *line, int (*compare_func) (int), void (*command_func)(readline_state_t *, string *, int (*compare_func)(int)), size_t n);
void (*rl_manage_matching_vi_mode(void (*matching_func)(readline_state_t *, string *), manage_rl_accessor mode))(readline_state_t *, string *);

void rl_autocomplete(readline_state_t *rl_state, string *line, Vars *shell_vars);

void switch_to_insert_mode(readline_state_t *rl_state);
void rl_switch_to_normal_mode(readline_state_t *rl_state);

// ── undo function ───────────────────────────────────────────────────
void rl_save_undo_state(string *line, readline_state_t *rl_state);
void rl_load_previous_state(string *line, readline_state_t *rl_state);
void rl_pop_undo_state(readline_state_t *rl_state);
// ──────────────────────────────────────────────────────────────────────

// ── Character access ────────────────────────────────────────────────
int rl_get_cursor_pos_on_line(readline_state_t *rl_state);

void rl_change_prev_char(readline_state_t *rl_state, string *line, char c);
void rl_change_current_char(readline_state_t *rl_state, string *line, char c);
void rl_change_next_char(readline_state_t *rl_state, string *line, char c);
void rl_change_n_char(readline_state_t *rl_state, string *line, char c, int n);

char rl_get_prev_char(readline_state_t *rl_state, string *line);
char rl_get_current_char(readline_state_t *rl_state, string *line);
char rl_get_next_char(readline_state_t *rl_state, string *line);
char rl_get_n_char(readline_state_t *rl_state, string *line, int n);

void rl_delete_curr_char(readline_state_t *rl_state, string *line);
void rl_delete_prev_char(readline_state_t *rl_state, string *line);
// ──────────────────────────────────────────────────────────────────────
//
// ── history_operation ───────────────────────────────────────────────
void up_history(readline_state_t *rl_state, string *line);
void down_history(readline_state_t *rl_state, string *line);
// ──────────────────────────────────────────────────────────────────────

// ── cursor_movement ─────────────────────────────────────────────────
void rl_handle_redo_reverse_match(readline_state_t *rl_state, string *line);

void rl_handle_find_next_key(readline_state_t *rl_state, string *line);
void rl_handle_find_prev_key(readline_state_t *rl_state, string *line);
void rl_handle_redo_previous_match(readline_state_t *rl_state, string *line);

void rl_move_back_by_char(readline_state_t *rl_state, string *line);
void rl_move_forward_by_char(readline_state_t *rl_state, string *line);

void rl_move_to_end(readline_state_t *rl_state, string *line);
void rl_move_to_start(readline_state_t *rl_state, string *line);
void rl_move_to_first_char(readline_state_t *rl_state, string *line);

void rl_move_to_previous_word_start_alnum(readline_state_t *rl_state, string *line);
void rl_move_to_previous_word_start_sp(readline_state_t *rl_state, string *line);
void rl_move_to_next_word_start_alnum(readline_state_t *rl_state, string *line);
void rl_move_to_next_word_start_sp(readline_state_t *rl_state, string *line);

void rl_move_to_next_word_end_alnum(readline_state_t *rl_state, string *line);
void rl_move_to_next_word_end_sp(readline_state_t *rl_state, string *line);
void rl_move_to_previous_word_end_alnum(readline_state_t *rl_state, string *line);
void rl_move_to_previous_word_end_sp(readline_state_t *rl_state, string *line);

void rl_move_to_n_index(readline_state_t *rl_state, string *line);

void rl_move_to_next_matching_char(readline_state_t *rl_state, string *line, rl_matching_mode mode);
void rl_move_to_prev_matching_char(readline_state_t *rl_state, string *line, rl_matching_mode mode);
// ──────────────────────────────────────────────────────────────────────
// ── clipboard operation ─────────────────────────────────────────────
void rl_copy_until_end(readline_state_t *rl_state, string *line);
void rl_copy_from_n_to_cursor(readline_state_t *rl_state, string *line, size_t n);
// ──────────────────────────────────────────────────────────────────────
// ── string operation ────────────────────────────────────────────────

void rl_replace_current_char(readline_state_t *rl_state, string *line);

void rl_paste_after_cursor(readline_state_t *rl_state, string *line);
void rl_paste_on_cursor(readline_state_t *rl_state, string *line);
void rl_delete_from_n_to_cursor(readline_state_t *rl_state, string *line, size_t n);
void rl_delete_until_end(readline_state_t *rl_state, string *line);

void rl_swap_char(readline_state_t *rl_state, string *line);
void rl_swap_word(readline_state_t *rl_state, string *line);

void rl_substitute_current_char(readline_state_t *rl_state, string *line);
void rl_substitute_line (readline_state_t *rl_state, string *line);

void rl_clear_line (string *line);
// ──────────────────────────────────────────────────────────────────────

int handle_printable_keys(readline_state_t *rl_state, char c, string *line, Vars *shell_vars);
void handle_vi_control(readline_state_t *rl_state, char c, string *line, rl_vi_controls_mode mode, Vars *shell_vars);

extern int last_action;


#endif
