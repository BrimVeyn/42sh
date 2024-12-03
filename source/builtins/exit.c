/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 11:53:12 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/03 17:12:06 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "ft_regex.h"
#include "utils.h"
#include "regex.h"
#include <stdlib.h>

//TODO: handle exit errors
static void err_exit(char *msg){
	ft_dprintf(2, "42sh: exit: %s", msg);
}

void builtin_exit(const SimpleCommandP *command, __attribute__((unused)) Vars *shell_vars) {
	if (!command->word_list->data[1]) exit(EXIT_SUCCESS);

	if (regex_match("[^0-9]", command->word_list->data[1]).is_found == true){
		err_exit("numeric argument required\n");
		close_all_fds();
		gc(GC_CLEANUP, GC_ALL);
		exit (2);
	}

	if (command->word_list->data[2]){
		err_exit("too many arguments\n");
		g_exitno = 1;
		return;
	}

	long exit_no = ft_atol(command->word_list->data[1]) % 256;
	close_all_fds();
	gc(GC_CLEANUP, GC_ALL);

	exit(exit_no);
}
