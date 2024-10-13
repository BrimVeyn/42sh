/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 14:33:24 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/13 11:00:50 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/exec.h"
#include "lexer.h"
#include "libft.h"
#include "parser.h"
#include "ft_regex.h"
#include "utils.h"
#include <linux/limits.h>
#include <regex.h>
#include <stdio.h>
#include <unistd.h>

static char **sort_env(StringList *env) {
	char **self = ft_calloc(env->size + 1, sizeof(char *));
	size_t size = env->size;
	for (size_t i = 0; i < size; i++) {
		self[i] = env->data[i];
	}

	for (size_t i = 0; i < size - 1; i++) {
		for (size_t j = i + 1; j < size; j++) {
			if (ft_strcmp(self[i], self[j]) > 0) {
				char *tmp = self[i];
				self[i] = self[j];
				self[j] = tmp;
			}
		}
	}
	return self;
}

static void print_export_p(StringList *env) {
	char **res = sort_env(env);
	for (size_t i = 0; res[i]; i++) {
		printf("export %s\n", res[i]);
	}
}

void builtin_export(const SimpleCommand *command, Vars *shell_vars) {
	size_t i = 1;
	size_t args_len = 0;
	for (; command->args[args_len]; args_len++) {}

	//TODO: update with regex to handle -pppppp -pppp -p...
	if (!ft_strcmp(command->args[1], "-p")) {
		if (args_len == 2) {
			print_export_p(shell_vars->env);
			return ;
		} else i += 1;
	}
	
	for (; command->args[i]; i++) {
		char *arg = command->args[i];
		char *equal_ptr = ft_strchr(arg, '=');
		size_t equal_pos = equal_ptr - arg;
		bool has_equal = (equal_ptr);
		size_t arg_len = ft_strlen(command->args[i]);
		regex_match_t match = regex_match("[_a-zA-Z][_a-zA-Z0-9]*", command->args[i]);
		size_t match_len = (match.is_found) ? match.re_end - match.re_start : -1;

		if ((has_equal && match_len == equal_pos) || (!has_equal && match_len == arg_len)) {
			string_list_add_or_update(shell_vars->env, command->args[i], GC_ENV);
		} else {
			INVALID_IDENTIFIER(command->args[i]);
			g_exitno = 1;
		}
	}
}
