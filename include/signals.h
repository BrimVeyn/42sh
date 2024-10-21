/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 15:11:07 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/21 16:01:13 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNAL_H
#define SIGNAL_H

#include <stdbool.h>
#include "lexer.h"

extern int g_signal;

typedef enum {
	SIG_PROMPT,
	SIG_HERE_DOC,
	SIG_EXEC,
} type_of_signals;

void	*init_prompt_and_signals(char *input, bool shell_is_interactive);
int		rl_event_dummy(void);
void	signal_manager(type_of_signals mode);
void	signal_exec_mode(void);
void	signal_prompt_mode(void);
void	signal_sigint_prompt(__attribute__((unused)) int code);
void	signal_sigint_exec(__attribute__((unused)) int code);

#endif // !SIGNAL_H

