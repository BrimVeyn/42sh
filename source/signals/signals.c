/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 16:29:50 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/02 14:15:32 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "signals.h"
#include "ft_readline.h"

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void signal_sigint_prompt(int code) {
	rl_done = 1;
	g_exitno = 128 + code;
	g_signal = code;

}

void signal_sigint_heredoc(int code) {
	rl_done = 2;
	g_exitno = 128 + code;
	printf("\n");
	g_signal = code;
}

void signal_sigterm_exec(int code) {
	fatal(NULL, 128 + code);
}

void signal_heredoc_mode(void) {
    struct sigaction sa;
    sa.sa_handler = signal_sigint_heredoc;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
}

void signal_prompt_mode(void) {
    struct sigaction sa;
    sa.sa_handler = signal_sigint_prompt;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

	signal(SIGTERM, signal_sigterm_exec);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
}


void signal_exec_mode(void) {
	signal(SIGTERM, signal_sigterm_exec);
	signal(SIGTSTP, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTTIN, SIG_DFL);
	signal(SIGTTOU, SIG_DFL);
	signal(SIGCHLD, SIG_DFL);
	signal(SIGPIPE, SIG_IGN);
	//FIX: sigpipe in non-interactive
}

void signal_sigtstp_script(int code) {
	(void) code;
	// Send SIGTSTP to the entire foreground process group
  // FIX: this is weird
	pid_t pgid = tcgetpgrp(STDIN_FILENO);
	if (pgid > 0) {
		killpg(pgid, SIGTSTP);
	}
}

// void signal_script_mode(void) {
// 	signal(SIGTSTP, SIG_DFL);
// 	signal(SIGINT, SIG_DFL);
// 	signal(SIGQUIT, SIG_DFL);
// 	signal(SIGCONT, SIG_DFL);
// 	signal(SIGTTIN, SIG_IGN);
// 	signal(SIGTTOU, SIG_IGN);
// 	signal(SIGCHLD, SIG_DFL);
// }
void signal_script_mode() {
    struct sigaction sa;

    // Reset SIGINT to default behavior
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // No special flags
    sigaction(SIGINT, &sa, NULL);

    // Reset other signals
    signal(SIGTSTP, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGCONT, SIG_DFL);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGCHLD, SIG_DFL);

    printf("Signal script mode set\n");
}

void signal_manager(type_of_signals mode) {
	// dprintf(2, "signal manager called with option %d\n", mode);
	switch(mode) {
		case SIG_PROMPT: signal_prompt_mode(); break;
		case SIG_EXEC: signal_exec_mode(); break;
		case SIG_HERE_DOC: signal_heredoc_mode(); break;
		case SIG_SCRIPT: signal_script_mode(); break;
		default: return;
	}
}
