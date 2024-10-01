/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 12:20:06 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/01 17:22:47 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <linux/limits.h>
#include <string.h>
#include <unistd.h>
#include "exec.h"
#include "libft.h"
#include "ft_regex.h"
#include "utils.h"

void builtin_echo(const SimpleCommand *command, __attribute__((unused)) Vars *shell_vars) {
	char buffer[MAX_WORD_LEN * 10] = {0};
	memset(buffer, '\0', MAX_WORD_LEN);
	bool newline = true;
	size_t i = 1;

	for (; command->args[i] && regex_match("^-n*$", command->args[i]).is_found ; i++) {
		newline = false;
	}

	for (; command->args[i]; i++) {
		ft_sprintf(buffer, "%s", command->args[i]);
		if (command->args[i + 1]) {
			ft_sprintf(buffer, " ");
		}
	}
	if (newline)
		ft_sprintf(buffer, "\n");
	ft_putstr_fd(buffer, STDOUT_FILENO);
	g_exitno = 0;
}
