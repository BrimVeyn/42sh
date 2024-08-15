/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:45:20 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/15 16:22:02 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

Lexer_p lexer_init(char *input) {
	Lexer_p lexer = ft_calloc(1, sizeof(Lexer));

	Lexer self = {
		.input = input,
		.position = 0,
		.read_position = 0,
		.ch = 0,
	};
	*lexer = self;
	lexer_read_char(lexer);

	return lexer;
}
