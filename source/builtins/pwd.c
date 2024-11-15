/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/03 16:45:47 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/15 15:53:13 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <linux/limits.h>
#include <unistd.h>
#include "exec.h"
#include "libft.h"
#include "utils.h"

void builtin_pwd(const SimpleCommandP *command, Vars *shell_vars) {
	//TODO: shall we handle -L and -P ?
	(void) command; (void) shell_vars;
	char *cwd = getcwd(NULL, PATH_MAX);
	if (!cwd) {
		//TODO: g_exitno = ???
		return ;
	}
	ft_putstr_fd(cwd, STDOUT_FILENO);
	ft_putstr_fd("\n", STDOUT_FILENO);
	FREE_POINTERS(cwd);
	g_exitno = 0;
	return ;
}
