/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jobs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/04 17:11:38 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/10 15:44:29 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"
#include "ft_regex.h"
#include "libft.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define ERROR_NO_SUCH_JOB(arg) dprintf(STDERR_FILENO, "42sh: jobs: %s: no such job\n", arg);

char *sigStr(const int sig) {
    static const struct {
        int signo;
        char *str;
    } table[] = {
        { 1, "SIGHUP" },       // Hangup
        { 2, "SIGINT" },       // Interrupt from keyboard
        { 3, "SIGQUIT" },      // Quit from keyboard
        { 4, "SIGILL" },       // Illegal Instruction
        { 5, "SIGTRAP" },      // Trace/breakpoint trap
        { 6, "SIGABRT" },      // Abort signal
        { 7, "SIGBUS" },       // Bus error
        { 8, "SIGFPE" },       // Floating-point exception
        { 9, "SIGKILL" },      // Kill signal
        { 10, "SIGUSR1" },     // User-defined signal 1
        { 11, "SIGSEGV" },     // Invalid memory reference
        { 12, "SIGUSR2" },     // User-defined signal 2
        { 13, "SIGPIPE" },     // Broken pipe: write to pipe with no readers
        { 14, "SIGALRM" },     // Timer signal from alarm
        { 15, "SIGTERM" },     // Termination signal
        { 16, "SIGSTKFLT" },   // Stack fault on coprocessor
        { 17, "SIGCHLD" },     // Child stopped or terminated
        { 18, "SIGCONT" },     // Continue if stopped
        { 19, "SIGSTOP" },     // Stop process
        { 20, "SIGTSTP" },     // Stop typed at terminal
        { 21, "SIGTTIN" },     // Terminal input for background process
        { 22, "SIGTTOU" },     // Terminal output for background process
        { 23, "SIGURG" },      // Urgent condition on socket
        { 24, "SIGXCPU" },     // CPU time limit exceeded
        { 25, "SIGXFSZ" },     // File size limit exceeded
        { 26, "SIGVTALRM" },   // Virtual alarm clock
        { 27, "SIGPROF" },     // Profiling timer expired
        { 28, "SIGWINCH" },    // Window resize signal
        { 29, "SIGIO" },       // I/O now possible
        { 30, "SIGPWR" },      // Power failure
        { 31, "SIGSYS" },      // Bad system call
    };

    for (size_t i = 0; i < sizeof(table) / sizeof(table[0]); i++) {
        if (sig == table[i].signo) {
            return table[i].str;
        }
    }
    return NULL;
}


void print_job_entry(char *buffer, const size_t idx, int opts) {
	if (opts <= 1)
		ft_sprintf(buffer, "[%ld]\t", job_list->data[idx]->id);
	if (opts >= 1)
		ft_sprintf(buffer, "%d\t", job_list->data[idx]->pgid);
	if (opts <= 1) {
		if (job_is_stopped(job_list->data[idx]))
			ft_sprintf(buffer, "Stopped(%s)\t%s\n", sigStr(job_list->data[idx]->sig), get_pipeline(job_list->data[idx]));
		else
			ft_sprintf(buffer, "Running\t%s\n", get_pipeline(job_list->data[idx]));
	}
	if (opts >= 2)
		ft_sprintf(buffer, "\n");
}

void builtin_jobs(const SimpleCommand *command, __attribute__((unused)) Vars *shell_vars) {
	char buffer[MAX_WORD_LEN] = {0};
	uint8_t opts = 0;
	bool	is_options = true;
	g_exitno = 0;

	size_t i = 1;
	while (is_options) {
		is_options = false;
		for (; command->args[i] && regex_match("^-l*$", command->args[i]).is_found ; i++) {
			opts |= 1; is_options = true;
		}
		for (; command->args[i] && regex_match("^-p*$", command->args[i]).is_found ; i++) {
			opts |= 2; is_options = true;
		}
		for (; command->args[i] && regex_match("^-[pl]*$", command->args[i]).is_found ; i++) {
			opts |= 3; is_options = true;
		}
	}

	if (!command->args[i]) {
		for (size_t i = 0; i < job_list->size; i++) {
			print_job_entry(buffer, i, opts);
		}
	} else {
		for (; command->args[i]; i++) {
			int maybe_num = ft_atoi(command->args[i]);
			if (!is_number(command->args[i]) || maybe_num == 0 || maybe_num > (int) job_list->size) {
				g_exitno = 1;
				ERROR_NO_SUCH_JOB(command->args[i]);
			} else {
				print_job_entry(buffer, maybe_num - 1, opts);
			}
		}
	}
	ft_putstr_fd(buffer, STDOUT_FILENO);
}
