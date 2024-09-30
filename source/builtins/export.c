/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 14:33:24 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/30 15:08:17 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/exec.h"
#include "libft.h"
#include "parser.h"
#include "ft_regex.h"
#include "utils.h"
#include <linux/limits.h>
#include <stdio.h>
#include <unistd.h>

void builtin_export(const SimpleCommand *command, Vars *shell_vars) {
	(void) shell_vars;
	size_t i = 0;

	//if -p shift
	
	for (i = 1; command->args[i]; i++) {
		if (regex_match("[_a-zA-Z][_a-zA-Z0-9]*=", command->args[i]).re_end != -1) {
			;
		}
	}
}
