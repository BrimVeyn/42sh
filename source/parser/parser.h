/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/27 13:45:31 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/28 14:43:41 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../lexer/lexer.h"

typedef struct {
	Lexer_p lexer;
	TokenList *curr_command;
	TokenList *peak_command;
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

Parser *parser_init(char *input);
void parser_get_next_command(Parser *self);
void parse_current(Parser *self);
void parser_print_state(Parser *self);
void parser_parse_all(Parser *self);

RedirectionList *redirection_list_init(void);
void redirection_list_add(RedirectionList *rl, Redirection *redirection);
void redirection_list_prepend(RedirectionList *rl, Redirection *redirection);
