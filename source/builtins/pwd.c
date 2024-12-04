/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/03 16:45:47 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/02 13:55:07 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <linux/limits.h>
#include <unistd.h>
#include "exec.h"
#include "libft.h"
#include "utils.h"

#define PWD_P 1
#define PWD_L 2

void builtin_pwd(__attribute__((unused)) const SimpleCommandP *command, __attribute__((unused)) Vars * const shell_vars) {
	int options = 0;
	bool accept_options = true;
	
	char **args = command->word_list->data;
	for (int i = 1;args[i]; i++){
		if (accept_options && args[i][0] == '-'){
			for (int j = 1; args[i][j]; j++){
				if (args[i][j] == 'L')
					options |= PWD_L;
				else if (args[i][j] == 'P')
					options |= PWD_P;
				else {
					ft_dprintf(2, "42sh: pwd: -%c: invalid option\n", args[i][j]);
					ft_dprintf(2, "pwd: usage: pwd [-LP]\n");
					g_exitno = 1;
					return;
				}
			}
		} else {
			accept_options = false;
		}
	}
	
	char *cwd = NULL;
	if (!(options & PWD_P)){
		cwd = getenv("PWD");
	}
	if (!cwd && options & PWD_P){
		cwd = getcwd(NULL, PATH_MAX);
	}
	if (!cwd) {
		g_exitno = 1;
		return ;
	}
	ft_putstr_fd(cwd, STDOUT_FILENO);
	ft_putstr_fd("\n", STDOUT_FILENO);
	if (options & PWD_P){
		FREE_POINTERS(cwd);
	}
	g_exitno = 0;
	return ;
}
