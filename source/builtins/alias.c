/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   alias.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 00:02:06 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/30 19:44:09 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "final_parser.h"
#include "ft_regex.h"
#include "libft.h"
#include "utils.h"

#include <stdio.h>
#include <unistd.h>

static void print_alias(StringList *env) {
	if (!env->size) return ;

	char ** const res = sort_env(env);
	for (size_t i = 0; res[i]; i++) {
		const char * const equal_ptr = ft_strchr(res[i], '=');
		const char * const id = ft_substr(res[i], 0, equal_ptr - res[i]);
		const char * const value = ft_substr(equal_ptr + 1, 0, ft_strlen(res[i]) - (equal_ptr - res[i]));
		printf("alias %s=\'%s\'\n", id, value);
	}
	free(res);
}

void builtin_alias(const SimpleCommandP *command, Vars * const shell_vars) {
	StringList *args = command->word_list;
	g_exitno = 0;


	if (args->size == 1)
		print_alias(shell_vars->alias);

	for (size_t i = 1; i < args->size; i++) {
		char *const arg = args->data[i];

		if (regex_match("^[_0-9a-zA-Z!%,@]*$", arg).is_found) {
			char *maybe_alias = string_list_get_value(shell_vars->alias, arg);
			if (maybe_alias) {
				printf("alias %s=\'%s\'\n", arg, maybe_alias);
			} else {
				ft_dprintf(STDERR_FILENO, "42sh: alias: %s: not found\n", arg);
				g_exitno = 1;
			}
		} else if (regex_match("^[_0-9a-zA-Z!%,@]+=", arg).is_found) {
			/*dprintf(2, "hey from alias !\n");*/
			string_list_add_or_update(shell_vars->alias, arg);
		} else {
			ft_dprintf(STDERR_FILENO, "42sh: alias: %s: not found\n", arg);
			g_exitno = 1;
		}
	}
	return;
}
