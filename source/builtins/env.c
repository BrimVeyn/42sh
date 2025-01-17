/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 16:02:37 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/30 19:36:08 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"

void builtin_env(__attribute__((unused)) const SimpleCommandP *command, Vars * const shell_vars) {
	string_list_print(shell_vars->env);
}
