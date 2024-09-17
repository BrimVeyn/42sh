/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:17:56 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/17 17:09:13 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
#define PARSER_H

#include "../../include/42sh.h"

#define UNEXPECTED_TOKEN_STR "42sh: syntax error near unexpected token "
#define UNCLOSED_SUBSHELL_STR "42sh: syntax error: unclosed subshell, expected "
#define UNCLOSED_COMMAND_SUB_STR "42sh: syntax error: unclosed command substitution, expected `)\n"
#define UNCLOSED_QUOTES_STR "42sh: syntax error: unclosed quotes, expected "

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
	Redirection		**r;
	uint16_t	size;
	uint16_t	capacity;
} RedirectionList;

typedef struct SimpleCommand {
	RedirectionList			*redir_list;
	char					*bin;
	char					**args;
} SimpleCommand;

char *parser_get_env_variable_value(char *name, StringList *env);
//-------------------SimpleCommand-----------------------//
SimpleCommand		*parser_parse_current(TokenList *tl, StringList *env);

//-------------------Here_doc-----------------------//
bool				heredoc_detector(TokenList *data);

//-----------------Redirection List----------------//
RedirectionList		*redirection_list_init(void);
void				redirection_list_add(RedirectionList *rl, Redirection *redirection);
void				redirection_list_prepend(RedirectionList *rl, Redirection *redirection);
void				add_redirection_from_token(RedirectionList **redir_list, const Token *el);

int parser_filename_expansion(TokenList *tl);

//-------------------Command substitution-------------//
bool parser_command_substitution(TokenList *tl, StringList *env);

//-------------------Parameter Expansion-------------//
bool				parser_parameter_expansion(TokenList *tl, StringList *env);
void				parser_skip_subshell(TokenList *list, int *it);
void			skip_cmdgrp(TokenList *self, TokenList *list, int *i);
bool is_end_cmdgrp(const TokenList *list, const int *it);
int get_command_sub_range_end(char *str, int *i);
int skip_subshell_str(char *str, int *i);

#endif // !PARSER_H
