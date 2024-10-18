/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 16:29:50 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/18 15:19:54 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "signals.h"
#include "ft_readline.h"

#include <stdio.h>
#include <signal.h>

int g_signal;

void signal_sigint_prompt(__attribute__((unused)) int code) {
	rl_done = 1;
	g_signal = code;

}

void signal_sigint_exec(int code) {
	printf("\n");
	g_signal = code;
}

void signal_prompt_mode(void) {
    struct sigaction sa;
    sa.sa_handler = signal_sigint_prompt;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
}

void signal_exec_mode(void) {
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTSTP, SIG_DFL);
	signal(SIGTTIN, SIG_DFL);
	signal(SIGTTOU, SIG_DFL);
	signal(SIGCHLD, SIG_DFL);
}

void signal_manager(type_of_signals mode) {
	switch(mode) {
		case SIG_PROMPT:
			signal_prompt_mode();
			break;
		case SIG_EXEC:
			signal_exec_mode();
			break;
		case SIG_HERE_DOC:
			break;
		default:
			return;
	}
}
