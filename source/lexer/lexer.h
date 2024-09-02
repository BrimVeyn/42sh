/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/29 12:55:38 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/02 09:17:09 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

#ifndef MAXISH_H
	#include "../../include/42sh.h"
#endif // !MAXI_SH

typedef enum {
	DEFAULT,
	DQUOTE,
} type_mode;

typedef struct {
	char		*input;
	uint16_t	input_len;
	uint16_t	position;
	uint16_t	read_position;
	char		ch;

} Lexer;

typedef Lexer * Lexer_p;

#pragma once
#include "lexer_enum.h"
#include "lexer_struct.h"

typedef struct TokenList {
	Token		**t;
	uint16_t	size;
	uint16_t	capacity;
} TokenList;

typedef struct {
	void **garbage;
	uint16_t size;
	uint16_t capacity;
} Garbage;

//-----------------Lexer------------------//
Token			*lexer_get_next_token(Lexer_p l, bool recursive_call, type_mode mode);
Lexer_p			lexer_init(char *input);
void			lexer_debug(Lexer_p lexer);
void			lexer_deinit(Lexer_p lexer);
void			lexer_read_char(Lexer_p l);
TokenList *lexer_lex_till(Lexer_p l, type_of_separator sep);

//-----------------Utils------------------//
bool			is_whitespace(char c);
bool			is_redirection_char(char c);
void			*ft_realloc(void *ptr, size_t oldSize, size_t nbEl, size_t elSize);
bool			is_number(char *str);
bool			next_token_is_redirection(Lexer_p l);
bool			is_fdable_redirection(Lexer_p l);

//----------------Token---------------------//
void			token_redirection_init(Token *token);
void			token_expression_init(Token *token);
void			token_command_grouping_init(Token *token);
void			token_word_init(Token *token);
Token			*token_empty_init(void);

//----------------TokenList----------------//
TokenList		*token_list_init(void);
void			token_list_add(TokenList *tl, Token *token);
void			token_list_add_list(TokenList *t1, TokenList *t2);
//---------------Debug---------------------//
void			tokenToString(Token *t, size_t offset);
void			tokenListToString(TokenList *tl);
void			tokenToStringAll(TokenList *t);
Token			*genNoneTok(void);


#endif // !LEXER_H
