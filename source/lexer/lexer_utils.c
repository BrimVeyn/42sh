/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/30 10:19:29 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/30 10:20:30 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

bool is_fdable_redirection(Lexer_p l) {
	const char *input_ptr = &l->input[l->position];
	if (!ft_strncmp(input_ptr, ">&", 2) ||
		!ft_strncmp(input_ptr, "<&", 2) ||
		ft_strchr("<", l->ch)) {
		return true;
	}
	return false;
}
