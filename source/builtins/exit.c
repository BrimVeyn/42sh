/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 16:02:52 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/15 15:45:05 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "ft_regex.h"
#include "utils.h"
#include "regex.h"
#include <stdlib.h>

//TODO: handle exit errors
//

static void err_exit(char *msg){
	ft_dprintf(2, "42sh: exit: %s", msg);
}

void builtin_exit(const SimpleCommand *command, __attribute__((unused)) Vars *shell_vars) {
	if (!command->args[1]) exit(EXIT_SUCCESS);

	if (regex_match("[^0-9]", command->args[1]).is_found == true){
		err_exit("numeric argument required\n");
		exit (2);
	}

	if (command->args[2]){
		err_exit("too many arguments\n");
		g_exitno = 1;
		return;
	}

	exit(ft_atol(command->args[1]) % 256);
}
