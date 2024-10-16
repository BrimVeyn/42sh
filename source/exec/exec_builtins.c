/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:12:52 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/17 14:35:17 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "libft.h"
#include <stdio.h>

typedef void (*builtin_func_t)(const SimpleCommand *, Vars *);

bool builtin_executer(const SimpleCommand *command, Vars *shell_vars) {
	if (!command->bin) 
		return false;

	static const struct {
		char *bin;
		builtin_func_t func;
	} map[] = {
		{"env",  &builtin_env}, {"set",  &builtin_set},
		{"echo",  &builtin_echo}, {"exit",  &builtin_exit},
		{"export",  &builtin_export}, {"hash",  &builtin_hash},
		{"type",  &builtin_type}, {"jobs",  &builtin_jobs},
		/*{"pwd", &builtin_pwd},*/ {"unset", &builtin_unset},
		{"fg", &builtin_fg},  {"bg", &builtin_bg},
	};

	int result_index = -1;
	for (size_t i = 0; i < sizeof(map) / sizeof(map[0]); i++) {
		if (!ft_strcmp(command->bin, map[i].bin)) {
			result_index = i;
			break;
		}
	}

	if (result_index != -1) {
		map[result_index].func(command, shell_vars);
		return true;
	}
	return false;
}

bool is_builtin(const char *bin) {
	static const char *builtins[] = {
		"echo", "cd", /*"pwd",*/ "export", "type",
		"unset", "env", "exit", "set", 
		"hash", "jobs", "fg", "bg",
	};

	for (size_t i = 0; i < sizeof(builtins) / sizeof(builtins[0]); i++) {
		if (!ft_strcmp(builtins[i], bin)) {
			return true;
		}
	}
	return false;
}
