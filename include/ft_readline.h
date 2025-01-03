/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_readline.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:15:30 by nbardavi          #+#    #+#             */
/*   Updated: 2025/01/03 10:28:38 by nbardavi         ###   ########.fr       */
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

typedef enum {
	RL_GET,
	RL_SET,
} manage_rl_state_mode;

typedef enum {
	RL_NO_OP,
	RL_REFRESH,
} rl_event;

typedef struct s_position{
    int x;
    int y;
} position_t;

typedef struct s_search_mode {
	char *word_found;
	int word_start;
	bool active;
} search_mode_t;

typedef struct s_readline_state {
	char *prompt;
	size_t prompt_size;
	position_t cursor_offset;
	position_t cursor;
	search_mode_t search_mode;
	bool interactive;
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

void update_line(readline_state_t *rl_state, string *line);
void update_cursor_x(readline_state_t *rl_state, string *line, ssize_t n);

size_t get_col(void);
size_t get_row(void);
void rl_print_prompt(int fd, readline_state_t *rl_state);

void move_cursor(int x, int y);

void set_cursor_position(readline_state_t *rl_state);
void print_history_values(HISTORY_STATE *history);

#include "final_parser.h"

rl_event handle_special_keys(readline_state_t *rl_state, string *line, Vars *shell_vars);
char *ft_readline(const char *prompt, Vars *shell_vars);
void init_history(Vars *shell_vars);
void add_history(const char *str, Vars *shell_vars);
void handle_history_config(HISTORY_STATE *history, Vars *shell_vars);

#endif
