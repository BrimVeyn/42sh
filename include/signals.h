/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 15:11:07 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/01 19:26:02 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNAL_H
#define SIGNAL_H

#include <stdbool.h>
#include "final_parser.h"

extern int g_signal;

typedef enum {
	SIG_PROMPT,
	SIG_HERE_DOC,
	SIG_EXEC,
	SIG_SCRIPT,
} type_of_signals;

void	*read_input_prompt(char *input, Vars *shell_vars);
int		rl_event_dummy(void);
void	signal_manager(type_of_signals mode);
void	signal_exec_mode(void);
void	signal_prompt_mode(void);
void	signal_sigint_prompt(__attribute__((unused)) int code);
void	signal_sigint_exec(__attribute__((unused)) int code);

#endif // !SIGNAL_H

