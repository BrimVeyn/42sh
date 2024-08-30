/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 15:09:36 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/30 15:39:53 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

void signal_sigint_prompt(__attribute__((unused)) int code) {
	rl_done = 1;
	g_signal = code;
}

void signal_sigint_exec(__attribute__((unused)) int code) {
	printf("\n");
	g_signal = code;
}

void signal_prompt_mode(void) {
	signal(SIGINT, signal_sigint_prompt);
	signal(SIGQUIT, SIG_IGN);
}

void signal_exec_mode(void) {
	signal(SIGINT, signal_sigint_exec);
}

void signal_manager(type_of_signals mode) {
	switch(mode) {
		case SIG_PROMPT:
			signal_prompt_mode();
			break;
		case SIG_EXEC:
			signal_exec_mode();
			break;
		default:
			return;
	}
}

int rl_event_dummy(void) { return 1; }
