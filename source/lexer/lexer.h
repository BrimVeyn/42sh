/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:07:36 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/28 15:55:22 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

#include "../../libftprintf/header/libft.h"
#include "../../include/colors.h"

#include <stdint.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <readline/history.h>

typedef enum {
	DEFAULT,
	DQUOTE,
} type_mode;

typedef struct {
	type_mode	mode;
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
TokenList		*lexer_lex_all(Lexer_p l);
Token			*lexer_get_next_token(Lexer_p l, bool recursive_call);
Lexer_p			lexer_init(char *input, type_mode mode);
void			lexer_debug(Lexer_p lexer);
void			lexer_deinit(Lexer_p lexer);
void			lexer_read_char(Lexer_p l);
TokenList *lexer_lex_till_operator(Lexer_p l);
TokenList *lexer_lex_till(Lexer_p l, type_of_separator sep);

//-----------------Utils------------------//
bool			is_whitespace(char c);
bool			is_redirection_char(char c);
bool			is_number(char c);
void			*ft_realloc(void *ptr, size_t oldSize, size_t nbEl, size_t elSize);

//----------------Token---------------------//
void			token_redirection_init(Token *token);
void			token_expression_init(Token *token);
void			token_command_grouping_init(Token *token);
void			token_word_init(Token *token);
Token			*token_empty_init(void);

//----------------Garbage-------------------//
void			gc_init(void);
void			*gc_add(void *ptr);
void			gc_cleanup(void);
//----------------TokenList----------------//
TokenList		*token_list_init(void);
void			token_list_add(TokenList *tl, Token *token);
//---------------Debug---------------------//
void			tokenToString(Token *t, size_t offset);
void			tokenListToString(TokenList *tl);
void			tokenToStringAll(TokenList *t);


#endif // !LEXER_H
