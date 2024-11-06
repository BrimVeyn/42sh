/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 10:05:45 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/06 10:05:45 by bvan-pae         ###   ########.fr       */
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
#define UNEXPECTED_EOF(c) dprintf(STDERR_FILENO, "42sh: unexpected EOF while looking for matching `%c'\n", c);
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
#define ERROR_NO_SUCH_JOB(builtin, arg) dprintf(STDERR_FILENO, "42sh: %s: %s: no such job\n", builtin, arg);

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

//-------------------Expands related-----------------------------//
typedef enum {
	EXP_ARITHMETIC,
	EXP_VARIABLE,
	EXP_CMDSUB,
	EXP_SUB,
	EXP_WORD,
} ExpKind;

typedef struct Str {
	struct Str *next;
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

typedef struct {
	ExpKind *data;
	int gc_level;
	size_t size;
	size_t capacity;
	size_t size_of_element;
} ExpKindList;

Str *str_init(const ExpKind kind, char *str, bool add_to_gc);
void				str_list_print(const StrList *list);
//---------------------------------------------------------------//

//-------------------SimpleCommand-----------------------//
SimpleCommand		*parser_parse_current(TokenList *tl, Vars *shell_vars);
bool				execute_command_sub(char *input, Vars *shell_vars);

//-------------------Here_doc-----------------------//
bool heredoc_detector(TokenList *data, Vars *shell_vars);

//-------------------Parser modules------------//
char				*parser_parameter_expansion(char *str, Vars *shell_vars);
char				*parser_command_substitution(char *str, Vars *shell_vars);
char				*parser_arithmetic_expansion(char *str, Vars *shell_vars);

//-------------------history modules------------//
bool				history_expansion (char **pstring, int history_fd);
void				add_input_to_history(char *input, int *history_fd);
int					get_history(void);

//-------------------Parameter Expansion-------------//
void				parser_skip_subshell(TokenList *list, size_t *it);
void				skip_cmdgrp(TokenList *self, TokenList *list, size_t *i);
//---------------------------------------------------------------//
//
#endif // !PARSER_H
