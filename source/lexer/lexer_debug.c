/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_debug.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 15:04:37 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/30 15:14:51 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

void lexer_debug(Lexer_p lexer) {
	printf("--- Lexer state ---\n");
	printf("Input = |%s|\n", lexer->input);
	printf("Position = %d\n", lexer->position);
	printf("Read_position = %d\n", lexer->read_position);
	printf("Ch = %c | %d\n", lexer->ch, lexer->ch);
	printf("-------------------\n");
}
