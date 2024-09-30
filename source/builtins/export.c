/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 14:33:24 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/30 16:26:43 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/exec.h"
#include "lexer.h"
#include "libft.h"
#include "parser.h"
#include "ft_regex.h"
#include "utils.h"
#include <linux/limits.h>
#include <stdio.h>
#include <unistd.h>

void builtin_export(const SimpleCommand *command, Vars *shell_vars) {
	size_t i = 0;
	size_t args_len = 0;
	for (; command->args[args_len]; args_len++) {}
	(void) args_len;

	// if (!ft_strcmp(command->args[1], "-p") && command->)
	
	for (i = 1; command->args[i]; i++) {
		if (regex_match("[_a-zA-Z][_a-zA-Z0-9]*=", command->args[i]).is_found) {
			string_list_add_or_update(shell_vars->env, command->args[i]);
		} else {

		}
	}
}
