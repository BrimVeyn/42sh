/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fc.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 11:40:48 by nbardavi          #+#    #+#             */
/*   Updated: 2024/11/25 13:50:13 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "utils.h"
#include "ft_readline.h"

void builtin_fc(const SimpleCommandP *command, Vars *shell_vars) {
	(void)command; (void)shell_vars;

	g_exitno = 0;
	return ;
}
