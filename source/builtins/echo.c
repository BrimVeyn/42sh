/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 12:20:06 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/30 13:44:50 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/exec.h"
#include "libft.h"
#include "parser.h"
#include "utils.h"
#include <linux/limits.h>
#include <stdio.h>
#include <unistd.h>

void builtin_echo(const SimpleCommand *command, __attribute__((unused)) Vars *shell_vars) {
	char buffer[NGROUPS_MAX] = {0};
	for (size_t i = 1; command->args[i]; i++) {
		ft_sprintf(buffer, "%s", command->args[i]);
		if (command->args[i + 1]) {
			ft_sprintf(buffer, " ");
		}
	}
	ft_sprintf(buffer, "\n");
	// printf("%s", buffer);
	ft_putstr_fd(buffer, STDOUT_FILENO);
	g_exitno = 0;
}
