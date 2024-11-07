/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 10:16:41 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/07 17:03:30 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


typedef enum {
	WORD_WORD,
	WORD_CMD_SUB,
	WORD_SUBSHELL,
	WORD_PARAM,
	WORD_ARITHMETIC,
	WORD_ARITHMETIC_PAREN,
	WORD_SINGLE_QUOTE,
	WORD_DOUBLE_QUOTE,
	NONE,
} WordContext;

typedef struct {
	unsigned char bit0 : 1; //DOUBLE_QUOTE
	unsigned char bit1 : 1; //SINGLE_QUOTE
	unsigned char bit2 : 1; //ARITHMETIC_PAREN
	unsigned char bit3 : 1; //ARITHMETIC
	unsigned char bit4 : 1; //PARAM
	unsigned char bit5 : 1; //SUBSHELL
	unsigned char bit6 : 1; //CMD_SUB
	unsigned char bit7 : 1; //WORD ignored
} BitMap8;

typedef struct {
	char *start;
	char *end;
	BitMap8 bitmap;
} WordContextBounds;

typedef struct {
	char	*data;
	size_t	size;
	size_t	capacity;
	size_t	size_of_element;
	int		gc_level;
} WordContextList;

#define WORD_MAP (BitMap8) {0, 1, 0, 1, 1, 0, 1, 1}
#define PARAM_MAP (BitMap8) {0, 0, 0, 1, 0, 0, 0, 0}
#define CMD_SUB_MAP (BitMap8) {0, 1, 1, 1, 1, 0, 1, 1}
#define SUBSHELL_MAP (BitMap8) {0, 1, 1, 1, 1, 0, 1, 1}
#define ARITHMETIC_MAP (BitMap8) {0, 1, 0, 1, 1, 1, 0, 0}
#define ARITHMETIC_PAREN_MAP (BitMap8) {0, 1, 0, 1, 1, 1, 0, 0}
#define SINGLE_QUOTE_MAP (BitMap8) {0, 0, 0, 0, 0, 0, 0, 0}
#define DOUBLE_QUOTE_MAP (BitMap8) {0, 1, 0, 1, 1, 0, 0, 0}

typedef enum {
	T_REDIRECTION,
	T_WORD, 
	T_SEPARATOR,
	T_FOR,
	T_WHILE,
	T_CASE,
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
	S_SUB_OPEN, // (
	S_SUB_CLOSE, // )
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

typedef struct {
	char		*input;
	short		ch;
	size_t	input_len;
	size_t	position;
	size_t	read_position;
} Lexer;

typedef Lexer * Lexer_p;

typedef struct {
	char		**data;
	size_t		size;
	size_t		capacity;
	size_t		size_of_element;
	int		gc_level;
} StringList;

typedef struct Token {
	type_of_token tag;
	type_of_error e;
	union {
		type_of_separator s_type;

		struct {
			struct Token *r_postfix;
			type_of_redirection r_type;
		};  //redirection

		struct {
			struct Token	*w_postfix;
			char			*w_infix;
		}; //word
	};
} Token;

typedef enum {
	SEP_AND,
	SEP_OR,
	SEP_SEMI,
	SEP_BG,
	SEP_NEWLINE,
	SEP_PIPE,
} SeparatorType;

typedef struct {
	SeparatorType type;
	char *litteral;
} Separator;

typedef struct {
	struct ExprArrayArray *conditions;
	struct ExprArrayArray *bodies;
	struct ExprArray *else_body;
} IfStruct;

typedef struct ExprArray {
	struct Expr	**data;
	size_t	size;
	size_t	capacity;
	size_t	size_of_element;
	int		gc_level;
} ExprArray;

typedef struct ExprArrayArray {
	struct ExprArray **data;
	size_t	size;
	size_t	capacity;
	size_t	size_of_element;
	int		gc_level;
} ExprArrayArray;

typedef struct {
	char *litteral;
} Word;

typedef struct Expr {
	enum {EXPR_WORD, EXPR_IF, EXPR_SEP, EXPR_SUB, EXPR_REDIR} type;
	union {
		// Redirection *redir;
		Word		*word;
		IfStruct  	*ifstruct;
		ExprArray 	*subshell;
		Separator 	*sep;
	};
} Expr;

typedef struct TokenList {
	Token		**data;
	size_t	size;
	size_t	capacity;
	size_t	size_of_element;
	int		gc_level;
} TokenList;

typedef struct {
	StringList *env;
	StringList *set;
	StringList *local;
} Vars;

typedef enum {
	H_NO_EVENT,
	H_LAST,
	H_WORD,
	H_NUMBER,
} type_of_history_event;


//-----------------Lexer------------------//
void parse_input(char *in, char *filename, Vars *shell_vars);
TokenList		*lexer_lex_all(char *input);
Token			*lexer_get_next_token(Lexer_p l);
Lexer_p			lexer_init(char *input);
void			lexer_debug(Lexer_p lexer);
void			lexer_deinit(Lexer_p lexer);
void			lexer_read_char(Lexer_p l);
void			lexer_read_x_char(Lexer_p l, uint16_t n);
bool			lexer_syntax_error(TokenList *tokens);


//-------------New Lexer--------------------//


//------------------------------------------//

//-----------------Utils------------------//
bool			is_whitespace(char c);
bool			is_number(char *str);
bool			next_token_is_redirection(Lexer_p l);
bool			is_fdable_redirection(Lexer_p l);
bool			is_delimiter(const char c);
void			eat_whitespace(Lexer_p l);
char			*get_word(Lexer_p l);

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
void			token_list_add_list(TokenList *t1, TokenList *t2);
void			token_list_remove(TokenList *tl, int index);
void			token_list_insert(TokenList *tl, Token *token, const int index);
void			token_list_insert_list(TokenList *dest, TokenList *src, const int index);

//----------------StringList----------------//
void			string_list_add_or_update(StringList *sl, char *var);
bool			string_list_update(StringList *sl, char *var);
void			string_list_append(StringList *sl, char *var);
void			string_list_clear(StringList *list);
bool			string_list_remove(StringList *sl, char *id);
char			*string_list_get_value(StringList *sl, char *id);
char			*shell_vars_get_value(Vars *shell_vars, char *id);
void			string_list_print(const StringList *list);

//----------------Syntax----------------//
bool	is_pipe(const TokenList *list, const size_t *i);
bool	is_newline(const TokenList *list, const size_t *i);
bool 	is_eof(const TokenList *list, const size_t *i);
bool 	is_semi(const TokenList *list, const size_t *i);
bool 	is_subshell(const TokenList *list, const size_t *i);
bool 	is_end_sub(const TokenList *list, const size_t *i);
bool 	is_or(const TokenList *list, const size_t *i);
bool 	is_bg(const TokenList *list, const size_t *i);
bool 	is_and(const TokenList *list, const size_t *i);
bool 	is_logical_operator(const TokenList *tokens, const size_t *it);
bool	is_break_seperator(const TokenList *tokens, const size_t *it);
bool 	is_separator(const TokenList *tokens, const size_t *it);
bool	is_word(const TokenList *list, const size_t *it);
bool	is_redirection_tag(const TokenList *list, const size_t *it);
bool	is_redirection(const TokenList *tokens, const size_t *it);
bool	is_semi_or_bg(const TokenList *list, const size_t *it);
bool	is_or_or_and(const TokenList *list, const size_t *it);
bool	is_ast_operator(const TokenList *list, const size_t *it);
bool	is_cmdgrp_start(const TokenList *list, const size_t *i);
bool	is_cmdgrp_end(const TokenList *list, const size_t *i);
bool	is_end_cmdgrp(const TokenList *list, const size_t *it);


#endif // !LEXER_H
