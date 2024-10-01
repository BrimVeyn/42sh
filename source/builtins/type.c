/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 11:02:12 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/01 11:47:08 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <linux/limits.h>
#include <unistd.h>
#include "exec.h"
#include "libft.h"
#include "parser.h"
#include "utils.h"
#include "ft_regex.h"

void builtin_type(const SimpleCommand *command, __attribute__((unused)) Vars *shell_vars) {
	char buffer[XATTR_NAME_MAX] = {0};
	size_t i = 1;

	for (; command->args[i]; i++) {
		ft_sprintf(buffer, "%s", command->args[i]);
		if (command->args[i + 1]) {
			ft_sprintf(buffer, "\n");
		}
	}

	ft_putstr_fd(buffer, STDOUT_FILENO);
	g_exitno = 0;
}
