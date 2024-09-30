/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 16:01:45 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/30 12:20:00 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/exec.h"
#include "parser.h"

void builtin_set(__attribute__((unused)) const SimpleCommand * command, Vars *shell_vars) {
	const StringList *set = shell_vars->set;
	string_list_print(set);
}
