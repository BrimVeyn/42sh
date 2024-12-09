/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 12:20:06 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/09 13:51:51 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "exec.h"
#include "libft.h"
#include "ft_regex.h"
#include "utils.h"


void builtin_echo(const SimpleCommandP *command, __attribute__((unused)) Vars * const shell_vars) {
	char buffer[MAX_WORD_LEN] = {0};
	bool newline = true;
	size_t i = 1;

	size_t buffer_size = 0;

	for (; command->word_list->data[i] && regex_match("^-n+$", command->word_list->data[i]).is_found ; i++) {
		newline = false;
	}

	for (; i < command->word_list->size; i++) {
		if (command->word_list->data[i + 1])
			buffer_size = ft_sprintf(buffer, "%s ", command->word_list->data[i]);
		else
			buffer_size = ft_sprintf(buffer, "%s", command->word_list->data[i]);
	}
	if (newline)
		buffer_size = ft_sprintf(buffer, "\n");
	if (write(STDOUT_FILENO, buffer, buffer_size) == -1){
		error("42sh: echo: write error: No space left on device", 1);
	}
	g_exitno = 0;
}
