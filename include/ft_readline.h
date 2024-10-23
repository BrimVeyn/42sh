/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_readline.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:15:30 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/23 14:52:30 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_READLINE_H
# define FT_READLINE_H


#ifndef C_STRING_H
# include "c_string.h"
#endif

#include <stdbool.h>

typedef struct _hist_entry {
  string line;
} HIST_ENTRY;

typedef struct _hist_state {
  HIST_ENTRY **entries; /* Pointer to the entries themselves. */
  int offset;           /* The location pointer within this array. */
  int length;           /* Number of elements within this array. */
  int capacity;             /* Number of slots allocated to this array. */
} HISTORY_STATE;

typedef enum {
	RL_GET,
	RL_SET,
} manage_rl_state_mode;

typedef struct s_position{
    int x;
    int y;
} position_t;

typedef struct s_search_mode {
	char *word_found;
	bool active;
} search_mode_t;

typedef struct s_readline_state {
	string prompt;
	position_t cursor_offset;
	position_t cursor;
	search_mode_t search_mode;
	bool interactive;
	// size_t offset_prompt;
	// size_t offset_x;
	// size_t offset_y;
    // int cursor_x;
    // int cursor_y;
} readline_state_t;
extern int rl_done;
extern HISTORY_STATE *history;
extern bool history_defined;

void init_history();
void destroy_history();
void add_history(const char *str);
void pop_history();
void str_info(const string *str);
void set_prompt(readline_state_t *rl_state, const char *new_prompt);
char *search_in_history(char *str);

char *ft_readline(const char *prompt);
void ft_readline_clean();
void ft_rl_newline();

#endif
