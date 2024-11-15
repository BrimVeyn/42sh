/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 16:02:52 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/15 15:53:55 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "utils.h"
#include <stdlib.h>

//TODO: handle exit errors
void builtin_exit(const SimpleCommandP *command, __attribute__((unused)) Vars *shell_vars) {
	if (!command->word_list->data[1]) exit(EXIT_SUCCESS);
	exit(ft_atol(command->word_list->data[1]) % 256);
}
