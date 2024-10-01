/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 16:02:52 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/01 10:57:33 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "utils.h"
#include <stdlib.h>

//TODO: handle exit errors
void builtin_exit(const SimpleCommand *command, __attribute__((unused)) Vars *shell_vars) {
	if (!command->args[1]) exit(EXIT_SUCCESS);
	exit(ft_atol(command->args[1]) % 256);
}
