/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_readline.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 16:15:30 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/22 10:20:41 by nbardavi         ###   ########.fr       */
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

extern int rl_done;
extern HISTORY_STATE *history;
extern bool history_defined;

void init_history();
void destroy_history();
void add_history(const char *str);
void pop_history();
void str_info(const string *str);
void set_prompt(const char *new_prompt);
char *search_in_history(char *str);

char *ft_readline(const char *prompt);
void ft_readline_clean();
void ft_rl_newline();

#endif
