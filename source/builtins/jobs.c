/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jobs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/04 17:11:38 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/04 17:31:34 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/exec.h"
#include "parser.h"

void builtin_jobs(__attribute__((unused)) const SimpleCommand *command, Vars *shell_vars) {
	(void) shell_vars;
	// job(TABLE_PRINT, 0);
}
