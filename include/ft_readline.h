/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_readline.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:15:30 by nbardavi          #+#    #+#             */
/*   Updated: 2025/01/14 16:18:29 by nbardavi         ###   ########.fr       */
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
} manage_rl_state_mode;

typedef enum {
	RL_NO_OP,
	RL_REFRESH,
} rl_event;

typedef enum {
    VI_INSERT,
    VI_NORMAL,
} rl_vi_mode;

typedef enum {
    RL_VI,
    RL_READLINE,
} rl_inline_mode;

typedef struct s_search_mode {
	char *word_found;
	int word_start;
	bool active;
} search_mode_t;

typedef struct s_inline {
    rl_inline_mode mode;
    rl_vi_mode vi_mode;
} inline_t;

typedef struct s_readline_state {
	char *prompt;
	size_t prompt_size;
	position_t cursor_offset;
	position_t cursor;
	search_mode_t search_mode;
    undo_state_stack_t *undo_stack;
	bool interactive;
    inline_t in_line;
} readline_state_t;

extern int rl_done;
extern HISTORY_STATE *history;
extern bool history_defined;

void destroy_history();
HIST_ENTRY *pop_history();
void str_info(const string *str);
void set_prompt(readline_state_t *rl_state, const char *new_prompt);
int search_in_history(readline_state_t *rl_state, char *str);

void handle_control_keys(readline_state_t *rl_state, char char_c);
int handle_printable_keys(readline_state_t *rl_state, char c, string *line);
int can_go_right(readline_state_t *rl_state, string *line);
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

rl_event handle_readline_controls(readline_state_t *rl_state, char c, string *line);
rl_event handle_special_keys(readline_state_t *rl_state, string *line);
char *ft_readline(const char *prompt, Vars *shell_vars);
void init_history(Vars *shell_vars);
void add_history(const char *str, Vars *shell_vars);
void handle_history_config(HISTORY_STATE *history, Vars *shell_vars);

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
// ──────────────────────────────────────────────────────────────────────
//
// ── history_operation ───────────────────────────────────────────────
rl_event up_history(readline_state_t *rl_state, string *line);
rl_event down_history(readline_state_t *rl_state, string *line);
// ──────────────────────────────────────────────────────────────────────

// ── cursor_movement ─────────────────────────────────────────────────
void rl_move_back_by_char(readline_state_t *rl_state, string *line);
void rl_move_forward_by_char(readline_state_t *rl_state, string *line);

void rl_move_to_end(readline_state_t *rl_state, string *line);
void rl_move_to_start(readline_state_t *rl_state, string *line);

void rl_move_to_next_word_start(readline_state_t *rl_state, string *line, int (*compare_func)(int));
void rl_move_to_previous_word_start(readline_state_t *rl_state, string *line, int (*compare_func)(int));
// ──────────────────────────────────────────────────────────────────────

// ── string operation ────────────────────────────────────────────────
void rl_swap_char(readline_state_t *rl_state, string *line);
void rl_swap_word(readline_state_t *rl_state, string *line);
// ──────────────────────────────────────────────────────────────────────

extern int last_action;


#endif
