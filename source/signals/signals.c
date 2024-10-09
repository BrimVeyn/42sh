/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 15:09:36 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/09 13:11:43 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <signal.h>
#include <readline/readline.h>
#include <unistd.h>
#include <stdbool.h>

#include "signals.h"

int g_signal;

void signal_sigint_prompt(int code) {
	rl_done = 1;
	g_signal = code;
}

void signal_sigint_exec(int code) {
	printf("\n");
	g_signal = code;
}

void signal_prompt_mode(void) {
	signal(SIGINT, signal_sigint_prompt);
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

int rl_event_dummy(void) { return 1; }
