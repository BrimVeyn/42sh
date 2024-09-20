/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:17:51 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/17 15:12:21 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define DELIMITERS_DEFAULT "|$;\0\n"
#define DELIMITERS_DQUOTE "$\0"

typedef enum {
	T_COMMAND,
	//---------------
	T_REDIRECTION, //ok
	//---------------
	T_GROUPING,
	//---------------
	T_WORD, // generic word ?
	//---------------
	T_SEPARATOR,
	//---------------
	T_NONE, // Generic none token when no prefix/suffix
} type_of_token;

typedef enum {
	ERROR_NONE, //error free 
	ERROR_UNEXPECTED_TOKEN, //<> ||| <<<< <<<>
	ERROR_UNCLOSED_SQ, //'....\0
	ERROR_UNCLOSED_DQ, //'....\0
	ERROR_UNCLOSED_CG, //".....\0
	ERROR_BAD_SUBSTITUTION, // cf invalid identifier
	ERROR_ESCAPED_SQ, //'..\'..'
} type_of_error;

typedef enum {
	S_AND,// &&
	S_OR, // ||
	S_PIPE, // |
	S_SEMI_COLUMN, // ;
	S_BG, // &
	S_NEWLINE, // \n
	S_EOF, // \0
	S_CMD_SUB, // $(
	S_SUB_OPEN, // (
	S_SUB_CLOSE, // )
	S_DQ, //"
	S_DEFAULT,
} type_of_separator;

typedef enum {
	R_OUTPUT, //>[ex|n]
	R_APPEND, //>>[ex|n]
	R_INPUT, //[?n]<[ex|n]
	R_HERE_DOC, //[?n]<<[delimiter]
	R_DUP_IN, //[?n]<&[ex|n]
	R_DUP_OUT, //[?n]>&[ex|n]
	R_DUP_BOTH, //&>[ex|n]
	R_DUP_BOTH_APPEND, //&>>[ex]
} type_of_redirection;

typedef enum {
	C_BUILTIN, //echo, etc...
	C_SHELL, //grep, etc...
} type_of_command;

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

typedef struct StringList {
	char		**value;
	uint16_t	size;
	uint16_t	capacity;
} StringList;

typedef struct Token {
	type_of_token tag;
	type_of_error e;
	union {
		struct {
			type_of_separator s_type;
		}; //separator

		struct {
			type_of_redirection r_type;
			struct Token *r_postfix;
		};  //redirection

		struct {
			char			*w_infix;
			struct Token	*w_postfix;
		}; //word
	};
} Token;

typedef struct TokenList {
	Token		**t;
	uint16_t	size;
	uint16_t	capacity;
} TokenList;

typedef struct {
	type_of_error	error;
	char			*error_message;
} Error;


//-----------------Lexer------------------//
TokenList		*lexer_lex_all(Lexer_p l);
Token			*lexer_get_next_token(Lexer_p l,type_mode mode);
Lexer_p			lexer_init(char *input);
void			lexer_debug(Lexer_p lexer);
void			lexer_deinit(Lexer_p lexer);
void			lexer_read_char(Lexer_p l);
void			lexer_read_x_char(Lexer_p l, uint16_t n);
bool			lexer_syntax_error(TokenList *tokens);

//-----------------Utils------------------//
bool			is_whitespace(char c);
bool			is_redirection_char(char c);
bool			is_number(char *str);
bool			next_token_is_redirection(Lexer_p l);
bool			is_fdable_redirection(Lexer_p l);
bool			is_fdable_redirection(Lexer_p l);
bool			is_delimiter(type_mode mode, char c);
void			*ft_realloc(void *ptr, size_t oldSize, size_t nbEl, size_t elSize);
void			eat_whitespace(Lexer_p l);
char			*get_word(Lexer_p l, type_mode mode);

//-----------------Typing------------------//
type_of_token		get_token_tag(Lexer_p l);
type_of_separator	get_separator_type(Lexer_p l);
type_of_redirection get_redirection_type(Lexer_p l);

//----------------Token---------------------//
Token			*token_empty_init(void);
Token			*token_none_init(void);
void			token_redirection_init(Token *token);
void			token_expression_init(Token *token);
void			token_command_grouping_init(Token *token);
void			token_word_init(Token *token);

//----------------TokenList----------------//
TokenList		*token_list_init(void);
void			token_list_add(TokenList *tl, Token *token);
void			token_list_add_list(TokenList *t1, TokenList *t2);
void			token_list_remove(TokenList *tl, int index);
void			token_list_insert(TokenList *tl, Token *token, const int index);
void			token_list_insert_list(TokenList *dest, TokenList *src, const int index);

//----------------StringList----------------//
void			get_env_variable_id(char *buffer, char *variable);
void			get_env_variable_value(char *buffer, char *variable);
int				get_env_variable_index(StringList *sl, char *variable);
StringList		*string_list_init(void);
void			string_list_add_or_update(StringList *sl, char *variable);
void			string_list_remove(StringList *sl, char *variable);
char			*string_list_get_value_with_id(StringList *sl, char *id);

//----------------Syntax----------------//
bool	is_pipe(const TokenList *list, const int *i);
bool	is_newline(const TokenList *list, const int *i);
bool 	is_eof(const TokenList *list, const int *i);
bool 	is_semi(const TokenList *list, const int *i);
bool 	is_subshell(const TokenList *list, const int *i);
bool 	is_end_sub(const TokenList *list, const int *i);
bool 	is_or(const TokenList *list, const int *i);
bool 	is_bg(const TokenList *list, const int *i);
bool 	is_and(const TokenList *list, const int *i);
bool 	is_logical_operator(const TokenList *tokens, const int *it);
bool	is_break_seperator(const TokenList *tokens, const int *it);
bool 	is_separator(const TokenList *tokens, const int *it);
bool	is_word(const TokenList *list, const int *it);
bool	is_redirection_tag(const TokenList *list, const int *it);
bool	is_redirection(const TokenList *tokens, const int *it);
bool	is_semi_or_bg(const TokenList *list, const int *it);
bool	is_or_or_and(const TokenList *list, const int *it);
bool	is_ast_operator(const TokenList *list, const int *it);
bool	is_cmdgrp_start(const TokenList *list, const int *i);
bool	is_cmdgrp_end(const TokenList *list, const int *i);
bool	is_end_cmdgrp(const TokenList *list, const int *it);


#endif // !LEXER_H
