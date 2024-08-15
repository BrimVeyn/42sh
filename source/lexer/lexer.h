/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:07:36 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/15 16:51:11 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

#include "../../libftprintf/header/libft.h"
#include "lexer_enum.h"
#include "lexer_struct.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct Lexer {
	char		*input;
	uint16_t	position;
	uint16_t	read_position;
	char		ch;

} Lexer;

typedef Lexer * Lexer_p;

Lexer_p lexer_init(char *input);
void	lexer_debug(Lexer_p lexer);
void	lexer_deinit(Lexer_p lexer);
Token	lexer_get_next_token(Lexer_p lexer);
void	lexer_read_char(Lexer_p l);

#endif // !LEXER_H
