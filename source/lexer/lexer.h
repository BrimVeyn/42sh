/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:07:36 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/20 16:18:21 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

#include "../../libftprintf/header/libft.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	char		*input;
	uint16_t	input_len;
	uint16_t	position;
	uint16_t	read_position;
	char		ch;
	uint16_t	peak_position;
	char		peak_ch;

} Lexer;

typedef Lexer * Lexer_p;

#include "lexer_enum.h"
#include "lexer_struct.h"

typedef struct {
	void **garbage;
	uint16_t size;
	uint16_t capacity;
} Garbage;

//-----------------Lexer------------------//
Token_or_Error	lexer_get_next_token(Lexer_p lexer);
Lexer_p			lexer_init(char *input);
void			lexer_debug(Lexer_p lexer);
void			lexer_deinit(Lexer_p lexer);
void			lexer_read_char(Lexer_p l);

//-----------------Utils------------------//
bool			is_whitespace(char c);
bool			is_redirection_char(char c);
bool			is_number(char c);
void			*ft_realloc(void *ptr, size_t oldSize, size_t nbEl, size_t elSize);

//----------------Token---------------------//
void			token_redirection_init(Token *token);
void			token_expression_init(Token *token);
Token			*token_empty_init(void);

//----------------Garbage-------------------//
void gc_init(void);
void *gc_add(void *ptr);
void gc_cleanup(void);


#endif // !LEXER_H
