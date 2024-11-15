/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/03 17:06:22 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/15 16:09:42 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/exec.h"
#include "lexer.h"
#include "parser.h"
#include <linux/limits.h>
#include <regex.h>
#include <unistd.h>

void builtin_unset(const SimpleCommandP *command, Vars *shell_vars) {
	for (size_t i = 1; command->word_list->data[i]; i++) {
		string_list_remove(shell_vars->env, command->word_list->data[i]);
		string_list_remove(shell_vars->set, command->word_list->data[i]);
	}
}
