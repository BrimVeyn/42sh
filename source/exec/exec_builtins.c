/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:12:52 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/19 12:21:32 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "libft.h"
#include "final_parser.h"

typedef void (*builtin_func_t)(const SimpleCommandP *, Vars * const);

bool execute_builtin(const SimpleCommandP *command, Vars *shell_vars) {
	if (!command->word_list->data[0]) 
		return false;

	static const struct {
		char *bin;
		builtin_func_t func;
	} map[] = {
		{"bg", &builtin_bg}, {"cd", &builtin_cd},
		{"echo", &builtin_echo}, {"env", &builtin_env},
		{"exit", &builtin_exit}, {"export", &builtin_export},
		{"fc", &builtin_fc}, {"fg", &builtin_fg},
		{"hash", &builtin_hash}, {"jobs", &builtin_jobs},
		{"pwd", &builtin_pwd}, {"return", &builtin_return},
		{"set", &builtin_set}, {"test", &builtin_test},
		{"type", &builtin_type}, {"unset", &builtin_unset},
	};

	int result_index = -1;
	for (size_t i = 0; i < sizeof(map) / sizeof(map[0]); i++) {
		if (*(command->word_list->data[0]) == *(map[i].bin) &&
			!ft_strcmp(command->word_list->data[0], map[i].bin))
		{
			result_index = i;
			break;
		}
	}

	if (result_index != -1) {
		//execute the matched builtin
		map[result_index].func(command, shell_vars);
		return true;
	}
	return false;
}

bool is_builtin(const char *bin) {

	if (!bin) return false;

	static const char *builtins[] = {
		"bg", "cd", "echo", "env", "exit",
		"export", "fc", "fg", "hash", "jobs",
		"pwd", "return", "set", "test", "type",
		"unset",
	};

	for (size_t i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++) {
		if (*(bin) == *(builtins[i]) && !ft_strcmp(builtins[i], bin)) {
			return true;
		}
	}
	return false;
}
