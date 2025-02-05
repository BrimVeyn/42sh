/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 10:46:53 by bvan-pae          #+#    #+#             */
/*   Updated: 2025/01/02 16:22:45 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "ft_regex.h"
#include "utils.h"
#include <stdlib.h>

static void err_exit(char *msg){
	ft_dprintf(2, "42sh: exit: %s\n", msg);
}

void builtin_exit(const SimpleCommandP *command, __attribute__((unused)) Vars *shell_vars) {
	if (!command->word_list->data[1]) {
		close_fd_set(FD_ALL);
		gc(GC_CLEANUP, GC_ALL);
		exit(EXIT_SUCCESS);
	}

	if (regex_match("[^0-9]", command->word_list->data[1]).is_found == true){
		err_exit("numeric argument required\n");
		close_fd_set(FD_ALL);
		gc(GC_CLEANUP, GC_ALL);
		exit(2);
	}

	if (command->word_list->data[2]){
		err_exit("too many arguments\n");
		g_exitno = 1;
		return;
	}

	if (ft_strlen(command->word_list->data[1]) > 19) {
		err_exit("numeric argument required\n");
		close_fd_set(FD_ALL);
		gc(GC_CLEANUP, GC_ALL);
		exit(2);
	}

	long exit_no = ft_atol(command->word_list->data[1]) % 256;
	close_fd_set(FD_ALL);
	gc(GC_CLEANUP, GC_ALL);

	exit(exit_no);
}
