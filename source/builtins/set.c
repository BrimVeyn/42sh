/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 16:01:45 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/13 11:36:00 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "parser.h"

void builtin_set(__attribute__((unused)) const SimpleCommand * command, Vars *shell_vars) {
	string_list_print(shell_vars->set);
}
