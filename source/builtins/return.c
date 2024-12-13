/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   return.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/13 11:33:19 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/13 12:47:47 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "utils.h"
#include "final_parser.h"

void builtin_return(const SimpleCommandP *command, UNUSED Vars * const shell_vars) {
	if (!g_functionCtx) {
		return error("42sh: return: can only `return' from a function", 2);
	}
	if (command->word_list->size >= 2) {
		int exitno = ft_atoi(command->word_list->data[1]);
		g_exitno = exitno;
	}
	g_functionCtx |= E_RETURN;
	return ;
}
