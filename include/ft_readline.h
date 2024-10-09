/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_readline.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 11:31:11 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/09 14:37:10 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_READLINE_H
# define FT_READLINE_H

typedef struct _hist_entry {
  char *line;
} HIST_ENTRY;

typedef struct _hist_state {
  HIST_ENTRY **entries; /* Pointer to the entries themselves. */
  int offset;           /* The location pointer within this array. */
  int length;           /* Number of elements within this array. */
  int capacity;             /* Number of slots allocated to this array. */
} HISTORY_STATE;

extern HISTORY_STATE *history;

void init_history();
void destroy_history();
void add_history(const char *str);

char *ft_readline(char *prompt);

#endif
