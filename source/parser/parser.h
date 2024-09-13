/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 11:21:10 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/13 14:09:50 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
#define PARSER_H

#include "../../include/42sh.h"
#define UNEXPECTED_TOKEN_STR "42sh: syntax error near unexpected token "
#define UNCLOSED_SUBSHELL_STR "42sh: syntax error: unclosed subshell, expected "
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
	struct SimpleCommand	*next;
} SimpleCommand;

SimpleCommand *parser_parse_current(TokenList *tl, char **env, int *saved_fds);
bool heredoc_detector(TokenList *data);
char *parser_get_env_variable_value(char *name, char **env);

//-----------------Redirection List----------------//
RedirectionList *redirection_list_init(void);
void redirection_list_add(RedirectionList *rl, Redirection *redirection);
void redirection_list_prepend(RedirectionList *rl, Redirection *redirection);

int parser_filename_expansion(TokenList *tl);

//-------------------Command substitution-------------//
bool parser_command_substitution(TokenList *tl, char **env, int *saved_fds);

//-------------------Parameter Expansion-------------//
bool parser_parameter_expansion(TokenList *tl, char **env);

#endif // !PARSER_H
