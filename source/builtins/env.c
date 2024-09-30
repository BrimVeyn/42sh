/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 16:02:37 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/30 12:00:36 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/exec.h"
#include "parser.h"

void builtin_env(__attribute__((unused)) const SimpleCommand *command, Vars *shell_vars) {
	const StringList *env = shell_vars->env;
	string_list_print(env);
}
