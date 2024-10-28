/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_get_next_token.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:19:12 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/28 09:58:17 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "parser.h"
#include "utils.h"
#include "libft.h"
#include "debug.h"
#include <stdint.h>
#include <stdio.h>

typedef enum {
	SEMI,
	AMPER,
	NEWLINE,
	ENDOFFILE,
} SeparatorOp;

typedef enum {
	AND_IF,
	OR_IF,
} OperatorType;

typedef enum {
	IF, THEN, FI
} Keyword;

typedef struct TermNode TermNode;
typedef struct AndOrNode AndOrNode;
typedef struct CommandNode CommandNode;
typedef struct SimpleCommandNode SimpleCommandNode;
typedef struct CompoundCommandNode CompoundCommandNode;

typedef struct {
	TermNode *term;
	TermNode *next;
} CompoundListNode;

struct TermNode {
	AndOrNode *and_or;
	SeparatorOp separator; // ';' | '&' | '\n'
	AndOrNode *next;
};

struct AndOrNode {
	CommandNode *command; // (simple_command | compound_command)*
	OperatorType operator; // "&&" | "||"
	CommandNode *next;
};

struct CommandNode {
	enum {SIMPLE_CMD, COMPOUND_CMD} type;
	union {
		SimpleCommandNode *simple_cmd;
		CompoundCommandNode *compound_cmd;
	};
};

struct SimpleCommandNode {
	char *word; // single word (echo | ls)
};

typedef struct {
	CompoundListNode *condition;
	CompoundListNode *body;
} IfClauseNode;

struct CompoundCommandNode {
	enum {IF_CLAUSE} type;
	IfClauseNode *if_clause;
	//.... while for until select etc
};

void get_next_token(StringStream *input, StringStream *cache, size_t *line, size_t *column);

void skip_whitespaces(StringStream *input, size_t *column) {
	while (ft_strchr(" \t", input->data[0])) {
		da_pop_front(input);
		(*column)++;
	}
}

TermNode *parse_term();
AndOrNode *parse_and_or();
CommandNode *parse_command();
SimpleCommandNode *parse_simple_command();
CompoundCommandNode *parse_compound_command();
IfClauseNode *parse_if_clause();

typedef enum {LEX_SET, LEX_GET, LEX_PEAK, LEX_DEBUG} LexMode;

typedef struct {
	StringStream *input;
	StringStream *peak;
	size_t line; 
	size_t column;
} Lex;

char *lex_interface(LexMode mode, void *input) {
	static Lex *lexer = NULL;

	if (mode == LEX_SET) {
		lexer = gc_unique(Lex, GC_SUBSHELL);
		da_create(tmp, StringStream, sizeof(char), GC_SUBSHELL);
		lexer->input = tmp;
		da_create(tmp2, StringStream, sizeof(char), GC_SUBSHELL);
		lexer->peak = tmp;
		ss_push_string(lexer->input, input);
	} else if (mode == LEX_GET) {
		if (lexer->peak->size) {
			return ss_get_owned_slice(lexer->peak);
		} else {
			da_create(tmp, StringStream, sizeof(char), GC_SUBSHELL);
			get_next_token(lexer->input, tmp, &lexer->line, &lexer->column);
			return tmp->data;
		}
	} else if (mode == LEX_PEAK) {
		if (lexer->peak->size) {
			return lexer->peak->data;
		} else {
			get_next_token(lexer->input, lexer->peak, &lexer->line, &lexer->column);
			return lexer->peak->data;
		}
	} else if (mode == LEX_DEBUG) {
		dprintf(2, "input: %s\n", lexer->input->data);
		dprintf(2, "peak: %s\n", lexer->peak->data);
	}
	return NULL;
}



bool is_sep() {
	char *peak = lex_interface(LEX_PEAK, NULL);
	if (!ft_strcmp(";", peak) || !ft_strcmp("&", peak) || !ft_strcmp("\n", peak)) {
		return true;
	}
	return false;
}

SeparatorOp parse_sep() {
	char *sep = lex_interface(LEX_GET, NULL);
	if (!ft_strcmp(";", sep))
		return SEMI;
	else if (!ft_strcmp("&", sep))
		return AMPER;
	else if (!ft_strcmp("\n", sep))
		return NEWLINE;
	else
		return -1;
}

bool is_operator() {
	char *peak = lex_interface(LEX_PEAK, NULL);
	if (!ft_strcmp("&&", peak) || !ft_strcmp("||", peak)) {
		return true;
	}
	return false;
}

OperatorType parse_operator() {
	char *sep = lex_interface(LEX_GET, NULL);
	if (!ft_strcmp("&&", sep))
		return AND_IF;
	else if (!ft_strcmp("&", sep))
		return OR_IF;
	else
		return -1;
}

bool is_simple_command() {
	char *peak = lex_interface(LEX_PEAK, NULL);
	if (ft_strcmp("if", peak)) {
		return true;
	}
	return false;
}

bool is_compound_command() {
	char *peak = lex_interface(LEX_PEAK, NULL);
	if (!ft_strcmp("if", peak)) {
		return true;
	}
	return false;
}




void lexer_tokenize(char *in) {
	lex_interface(LEX_SET, in);
	CompoundListNode *root = NULL;
	dprintf(2, "tok: %s\n", root->term->and_or->command->simple_cmd->word);
	(void) root;
	//print this shit;
}
