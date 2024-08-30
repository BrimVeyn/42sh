/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 09:13:03 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/29 12:58:53 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

bool next_token_is_redirection(Lexer_p l) {
	const char *input_ptr = &l->input[l->position];
	if (!ft_strncmp(input_ptr, "&>", 2) ||
		!ft_strncmp(input_ptr, ">&", 2) ||
		!ft_strncmp(input_ptr, "<&", 2) ||
		!ft_strncmp(input_ptr, ">>", 2) ||
		!ft_strncmp(input_ptr, "<<", 2) ||
		ft_strchr("<>", l->ch)) {
		return true;
	}
	return false;
}
