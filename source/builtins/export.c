/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 11:16:25 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/30 00:45:34 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "final_parser.h"
#include "libft.h"
#include "parser.h"
#include "ft_regex.h"
#include "utils.h"
#include <limits.h>
#include <regex.h>
#include <stdio.h>
#include <unistd.h>

char **sort_env(StringList *env) {
	char ** const self = ft_calloc(env->size + 1, sizeof(char *));
	ft_memcpy(self, env->data, env->size * sizeof(char *));

	for (size_t i = 0; i < env->size - 1; i++) {
		for (size_t j = i + 1; j < env->size; j++) {
			if (ft_strcmp(self[i], self[j]) > 0) {
				char * const tmp = self[i];
				self[i] = self[j];
				self[j] = tmp;
			}
		}
	}
	return self;
}

static void print_export_p(StringList *env) {
	if (!env->size) return ;

	char ** const res = sort_env(env);
	for (size_t i = 0; res[i]; i++) {
		const char * const equal_ptr = ft_strchr(res[i], '=');
		const char * const  id = ft_substr(res[i], 0, equal_ptr - res[i]);
		const char * const value = ft_substr(equal_ptr + 1, 0, ft_strlen(res[i]) - (equal_ptr - res[i]));
		printf("export %s=\'%s\'\n", id, value);
	}
	free(res);
}

void builtin_export(const SimpleCommandP *command, Vars *shell_vars) {
	size_t i = 1;
	size_t args_len = 0;
	for (; command->word_list->data[args_len]; args_len++) {}

	for (; command->word_list->data[i] && regex_match("^-p+$", command->word_list->data[i]).is_found ; i++) {
		if (args_len == i + 1) {
			print_export_p(shell_vars->env);
			return ;
		}
	}
	
	for (; command->word_list->data[i]; i++) {

		char * const arg = command->word_list->data[i];
		const size_t arg_len = ft_strlen(arg);
		char buffer[MAX_WORD_LEN] = {0};

		if (regex_match("^[_a-zA-Z][_a-zA-Z0-9]*$", arg).is_found) {
			const char * const maybe_value = string_list_get_value(shell_vars->set, arg);

			if (maybe_value) {
				if (ft_snprintf(buffer, MAX_WORD_LEN, "%s=%s", arg, maybe_value) == -1)
					_fatal("snprintf: buffer overflow", 1);
				string_list_add_or_update(shell_vars->env, buffer);
			} else {
				string_list_add_or_update(shell_vars->env, arg);
				string_list_add_or_update(shell_vars->set, arg);
			}

		} else if (regex_match("^[_a-zA-Z][_a-zA-Z0-9]*=$", arg).is_found
			|| regex_match("^[_a-zA-Z][_a-zA-Z0-9]*=.+", arg).is_found) {
			string_list_add_or_update(shell_vars->env, arg);
			string_list_add_or_update(shell_vars->set, arg);
		} else if (regex_match("^[_a-zA-Z][_a-zA-Z0-9]*\\+=$", arg).is_found) {
			const char * const id = gc(GC_ADD, ft_substr(arg, 0, arg_len - 2), GC_SUBSHELL);
			const char * const maybe_value = string_list_get_value(shell_vars->env, id);

			if (maybe_value) {
				continue;
			} else {
				string_list_add_or_update(shell_vars->env, id);
				string_list_add_or_update(shell_vars->set, id);
			}

		} else if (regex_match("^[_a-zA-Z][_a-zA-Z0-9]*\\+=.+", arg).is_found) {
			const char * plus_ptr = ft_strchr(arg, '+');
			const char * const id = gc(GC_ADD, ft_substr(arg, 0, plus_ptr - arg), GC_SUBSHELL);
			const char * const maybe_value = string_list_get_value(shell_vars->set, id);
			const char *additional_value = gc(GC_ADD, ft_substr(arg, ((plus_ptr - arg) + 2), (arg_len - (plus_ptr - arg) - 2)), GC_SUBSHELL);

			if (maybe_value) {
				if (ft_snprintf(buffer, MAX_WORD_LEN, "%s=%s%s", id, maybe_value, additional_value) == -1)
					_fatal("snprintf: buffer overflow", 1);
			} else {
				if (ft_snprintf(buffer, MAX_WORD_LEN, "%s=%s", id, additional_value) == -1)
					_fatal("sprintf: buffer overflow", 1);
			}
			string_list_add_or_update(shell_vars->env, buffer);
			string_list_add_or_update(shell_vars->set, buffer);

		} else {
			INVALID_IDENTIFIER(command->word_list->data[i]);
			g_exitno = 1;
		}
	}
}
