/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 15:11:07 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/14 20:39:12 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNAL_H
#define SIGNAL_H

extern int g_signal;

typedef enum {
	SIG_PROMPT,
	SIG_HERE_DOC,
	SIG_EXEC,
} type_of_signals;

char	*init_prompt_and_signals(void);
int		rl_event_dummy(void);
void	signal_manager(type_of_signals mode);
void	signal_exec_mode(void);
void	signal_prompt_mode(void);
void	signal_sigint_prompt(__attribute__((unused)) int code);
void	signal_sigint_exec(__attribute__((unused)) int code);

#endif // !SIGNAL_H

