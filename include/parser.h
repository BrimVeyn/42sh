/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 16:23:25 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/14 16:23:35 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stdio.h>

#include "lexer.h"

//-----------------------------SHELL ------------------------------------------------------------------
#define UNEXPECTED_TOKEN_STR "42sh: syntax error near unexpected token "
#define UNCLOSED_SUBSHELL_STR "42sh: syntax error: unclosed subshell, expected "
#define UNCLOSED_COMMAND_SUB_STR "42sh: syntax error: unclosed command substitution, expected `)\n"
#define UNCLOSED_QUOTES_STR "42sh: syntax error: unclosed quotes, expected "
//------------------------------------------------------------------------------------------------------

//-----------------------------ARITHMETIC EXPANSION----------------------------------------------------
#define UNCLOSED_ARITMETIC_EXP_STR "42sh: syntax error: unclosed arithmetic expression, expected `))\n"
#define DIVISION_BY_0 "42sh: division by 0 (error token is \"0\")"
#define ASSIGNMENT_REQUIRES_LVALUE "42sh: assignment requires lvalue (error token is \""
#define OPERAND_EXPECTED "42sh: operand expected (error token is an operator)"
#define ARITHMETIC_SYNTAX_ERROR(s) dprintf(STDERR_FILENO, "42sh: sytax error: invalid arithmetic operator (error token is \"%s\")\n", s);
#define ARITHMETIC_UNCLOSED_PAREN(s) dprintf(STDERR_FILENO, "42sh: sytax error: unclosed parenthesis, expected `%s\"\n", s);
//------------------------------------------------------------------------------------------------------

//-----------------------------HASH TABLE---------------------------------------------------------------
#define INVALID_IDENTIFIER(s) dprintf(STDERR_FILENO, "bash: export: `%s\': not a valid identifier\n", s);
#define HASH_BIN_NOT_FOUND(s) dprintf(STDERR_FILENO, "bash: hash: %s: not found\n", s);
#define HASH_OPTION_REQUIRES_ARG dprintf(STDERR_FILENO, "bash: hash: -d: option requires an argument\n"); 
//------------------------------------------------------------------------------------------------------

//------------------------------Redirections---------------------//

typedef enum {
	R_FD,
	R_FILENAME,
} type_of_suffix;

typedef struct {
	int prefix_fd;
	type_of_redirection r_type;
	type_of_suffix su_type;
	union {
		int fd;
		char *filename;
	};
} Redirection;

typedef struct RedirectionList {
	Redirection		**data;
	size_t	size;
	size_t	capacity;
	size_t	size_of_element;
	int		gc_level;
} RedirectionList;

void add_redirection_from_token(RedirectionList *redir_list, const Token *el);

//---------------------------------------------------------------//

typedef struct SimpleCommand {
	RedirectionList			*redir_list;
	char					*bin;
	char					**args;
} SimpleCommand;

typedef enum {
	EXP_ARITHMETIC,
	EXP_VARIABLE,
	EXP_CMDSUB,
	EXP_SUB,
	EXP_WORD,
} ExpKind;

typedef struct {
	char *str;
	ExpKind kind;
} Str;

typedef struct {
	Str		**data;
	size_t	size;
	size_t	capacity;
	size_t	size_of_element;
	int		gc_level;
} StrList;

Str					*str_init(const ExpKind kind, char *str);
void				str_list_print(const StrList *list);

char				*parser_get_env_variable_value(char *name, StringList *env);
//-------------------SimpleCommand-----------------------//
SimpleCommand		*parser_parse_current(TokenList *tl, Vars *shell_vars);

//-------------------Here_doc-----------------------//
bool				heredoc_detector(TokenList *data);

//-------------------Parser modules------------//
bool				parser_parameter_expansion(TokenList *tl, Vars *shell_vars);
char				*parser_command_substitution(char *str, Vars *shell_vars);
bool				parser_arithmetic_expansion(TokenList *tokens, const int idx, const size_t start, Vars *shell_vars);
int					parser_filename_expansion(TokenList *tl);
bool				parser_word_split(TokenList *dest, Vars *shell_vars, char *prefix, char *infix, char *postfix, int index);

//-------------------history modules------------//
bool				history_expansion (char **pstring, int history_fd);
int					get_history(void);
void				add_input_to_history(char *input, int *history_fd);

//-------------------Parameter Expansion-------------//
void				parser_skip_subshell(TokenList *list, size_t *it);
bool				is_end_cmdgrp(const TokenList *list, const size_t *it);
void				skip_cmdgrp(TokenList *self, TokenList *list, size_t *i);
int					get_command_sub_range_end(char *str, int *i);

#endif // !PARSER_H
