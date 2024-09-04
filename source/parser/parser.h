/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:01:49 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/03 17:16:56 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
#define PARSER_H

#include "../../include/42sh.h"
#define UNEXPECTED_TOKEN_STR "42sh: syntax error near unexpected token "
#define UNCLOSED_SUBSHELL "42sh: syntax error: unclosed subshell, expected "
#define UNCLOSED_QUOTES "42sh: syntax error: unclosed quotes, expected "

typedef struct {
	type_of_error e;
	Lexer_p lexer;
	uint16_t it;
	struct TokenList *data;
	struct TokenList *curr_command;
	struct TokenList *peak_command;
} Parser;

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

bool syntax_error_detector(Parser *p);
bool heredoc_detector(TokenList *data);
Parser *parser_init(char *input);
void parser_get_next_command(Parser *self);
void parse_current(Parser *self);
void parser_print_state(Parser *self);
void parser_parse_all(Parser *self, char **env);
SimpleCommand *parser_parse_current(TokenList *tl);
void printCommand(SimpleCommand *command);
TokenList *lexer_lex_till_operator(Parser *p);
TokenList *lexer_lex_all(Lexer_p l);

RedirectionList *redirection_list_init(void);
void redirection_list_add(RedirectionList *rl, Redirection *redirection);
void redirection_list_prepend(RedirectionList *rl, Redirection *redirection);

type_of_separator interface_separator(type_of_separator new_separator, int mode);

#endif // !PARSER_H
