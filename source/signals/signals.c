/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 11:14:33 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/11 11:46:53 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "signals.h"
#include "ft_readline.h"
#include "jobs.h"

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void signal_sigint_prompt(int code) {
	rl_done = 1;
	g_exitno = 128 + code;
}

void signal_sigint_heredoc(int code) {
	rl_done = 2;
	g_exitno = 128 + code;
	printf("\n");
}

void signal_sigterm_exec(int code) {
	_fatal(NULL, 128 + code);
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
	signal(SIGQUIT, job_killall);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
}


void signal_exec_mode(void) {
	signal(SIGTERM, signal_sigterm_exec);
	signal(SIGTTIN, SIG_DFL);
	signal(SIGTTOU, SIG_DFL);
	signal(SIGTSTP, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGCHLD, SIG_DFL);
	signal(SIGPIPE, SIG_DFL);
}

void signal_script_mode() {
    struct sigaction sa;

    // Reset SIGINT to default behavior
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // No special flags
	sa.sa_handler = signal_sigterm_exec;
    sigaction(SIGINT, &sa, NULL);

    // Reset other signals
    signal(SIGTSTP, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGCONT, SIG_DFL);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGCHLD, SIG_DFL);
}

void signal_manager(const type_of_signals mode) {
	switch(mode) {
		case SIG_PROMPT: signal_prompt_mode(); break;
		case SIG_EXEC: signal_exec_mode(); break;
		case SIG_HERE_DOC: signal_heredoc_mode(); break;
		case SIG_SCRIPT: signal_script_mode(); break;
		default: return;
	}
}
