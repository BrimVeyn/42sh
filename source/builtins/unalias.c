/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unalias.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/30 19:35:57 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/31 13:27:05 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "final_parser.h"
#include "utils.h"
#include <unistd.h>

void builtin_unalias(const SimpleCommandP *command, Vars * const shell_vars) {
	StringList *args = command->word_list;
	g_exitno = 0;

	bool erase_all = false;

	for (size_t i = 1; i < args->size; i++) {
		const char *const arg = args->data[i];

		if (*arg == '-' && (*(arg + 1)) != '\0') {
			for (size_t i = 1; arg[i]; i++) {
				if (arg[i] == 'a') {
					erase_all = true;
				} else {
					ft_dprintf(STDERR_FILENO, "42sh: unalias: -%c: invalid option\n", arg[i]);
					g_exitno = 2;
					break;
				}
			}
			if (g_exitno == 2)
				break;
		}
		if (erase_all) {
			string_list_clear(shell_vars->alias);
			break;
		}
		if (!string_list_remove(shell_vars->alias, arg)) {
			ft_dprintf(STDERR_FILENO, "42sh: unalias: %s: not found\n", arg);
			g_exitno = 1;
		}
	}
	return ;
}
