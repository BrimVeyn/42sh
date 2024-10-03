/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/03 17:06:22 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/03 17:24:31 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/exec.h"
#include "lexer.h"
#include "parser.h"
#include <linux/limits.h>
#include <regex.h>
#include <unistd.h>

void builtin_unset(const SimpleCommand *command, Vars *shell_vars) {
	for (size_t i = 1; command->args[i]; i++) {
		string_list_remove(shell_vars->env, command->args[i]);
		string_list_remove(shell_vars->set, command->args[i]);
	}
}
