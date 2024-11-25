/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 16:01:45 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/25 16:01:05 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "parser.h"

void builtin_set(__attribute__((unused)) const SimpleCommandP * command, Vars * const shell_vars) {
	string_list_print(shell_vars->set);
}
