/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:12:52 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/30 19:39:16 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "libft.h"
#include "final_parser.h"
#include "utils.h"

typedef void (*builtin_func_t)(const SimpleCommandP *, Vars * const);

bool execute_builtin(const SimpleCommandP *command, Vars *const shell_vars) {
	if (!command->word_list->data[0]) 
		return false;

	static const struct {
		const char *bin;
		const builtin_func_t func;
	} table[] = {
		{"alias", &builtin_alias}, {"bg", &builtin_bg}, {"cd", &builtin_cd},
		{"echo", &builtin_echo}, {"env", &builtin_env},
		{"exit", &builtin_exit}, {"export", &builtin_export},
		{"fc", &builtin_fc}, {"fg", &builtin_fg},
		{"hash", &builtin_hash}, {"jobs", &builtin_jobs},
		{"pwd", &builtin_pwd}, {"return", &builtin_return},
		{"set", &builtin_set}, {"test", &builtin_test},
		{"type", &builtin_type}, {"unalias", &builtin_unalias},
		{"unset", &builtin_unset},
	};

	for (size_t i = 0; i < ARRAY_SIZE(table); i++) {
		if (*(command->word_list->data[0]) == *(table[i].bin) &&
			!ft_strcmp(command->word_list->data[0], table[i].bin))
		{
			table[i].func(command, shell_vars);
			return true;
		}
	}
	return false;
}

bool is_builtin(const char *const bin) {

	if (!bin) return false;

	static const char *const builtins[] = {
		"alias", "bg", "cd", "echo", "env", "exit",
		"export", "fc", "fg", "hash", "jobs",
		"pwd", "return", "set", "test", "type",
		"unalias", "unset",
	};

	for (size_t i = 0; i < ARRAY_SIZE(builtins); i++) {
		if (*(bin) == *(builtins[i]) && !ft_strcmp(builtins[i], bin)) {
			return true;
		}
	}
	return false;
}
