/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_jobs_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/23 22:23:54 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/23 22:24:44 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "libft.h"

bool is_whitespace(char c) {
	return (c == ' ' || c == '\t');
}

bool is_number(char *str) {
	for (uint16_t i = 0; str[i]; i++) {
		if (!ft_isdigit(str[i])) return false;
	}
	return true;
}
