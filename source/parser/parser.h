/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:01:49 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/15 11:01:05 by bvan-pae         ###   ########.fr       */
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

typedef struct {
	int start;
	int end;
} Range;

//-------------------SimpleCommand-----------------------//
SimpleCommand		*parser_parse_current(TokenList *tl, char **env);

//-------------------Here_doc-----------------------//
bool				heredoc_detector(TokenList *data);

//-----------------Redirection List----------------//
RedirectionList		*redirection_list_init(void);
void				redirection_list_add(RedirectionList *rl, Redirection *redirection);
void				redirection_list_prepend(RedirectionList *rl, Redirection *redirection);
void				add_redirection_from_token(RedirectionList **redir_list, const Token *el);

//-------------------Command substitution-------------//
bool				parser_command_substitution(TokenList *tl, char **env);
void				parser_skip_subshell(TokenList *list, int *it);


#endif // !PARSER_H
