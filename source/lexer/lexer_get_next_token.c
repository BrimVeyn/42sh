/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_get_next_token.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:19:12 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/26 16:34:30 by bvan-pae         ###   ########.fr       */
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

void parse_linebreak() {
	while (!ft_strcmp("\n", lex_interface(LEX_PEAK, NULL))) {
		lex_interface(LEX_GET, NULL);
	}
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
		return AMPER;
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

CompoundListNode *parse_compound_list() {
	CompoundListNode *node = gc_unique(CompoundListNode, GC_SUBSHELL);
	node->term = NULL;

	parse_linebreak();

	TermNode *term = parse_term();
	if (!term) {
		fatal("Compoundlist Term", 1);
	}

	node->term = term;
	
	if (is_sep()) {
		parse_sep();
	}
	return node;
}

TermNode *parse_term() {
	TermNode *node = gc_unique(TermNode, GC_SUBSHELL);
	node->separator = SEMI;
	node->next = NULL;
	node->and_or = parse_and_or();

	if (!node->and_or) {
		fatal("Term and_or", 1);
	}

	if (is_sep()) {
		node->separator = parse_sep();
		parse_linebreak();
		node->next = parse_and_or();
	}
	return node;
}

AndOrNode *parse_and_or() {
	AndOrNode *node = gc_unique(AndOrNode, GC_SUBSHELL);
	node->command = parse_command();
	node->next = NULL;

	if (!node->command) {
		fatal("AndOr command\n", 1);
	}

	while (is_operator()) {
		OperatorType op = parse_operator();
		parse_linebreak();
		AndOrNode *next_node = gc_unique(AndOrNode, GC_SUBSHELL);
		next_node->command = parse_command();
		next_node->operator = op;
		next_node->next = NULL;

		if (!next_node->command) {
			fatal("AndOr commandNext\n", 1);
		}
	}
	return node;
}

CommandNode *parse_command() {
	CommandNode *node = gc_unique(CommandNode, GC_SUBSHELL);

	if (is_simple_command()) {
		node->type = SIMPLE_CMD;
		node->simple_cmd = parse_simple_command();
		if (!node->compound_cmd) {
			fatal("Command simple command\n", 1);
		}
	} else if (is_compound_command()) {
		node->type = COMPOUND_CMD;
		node->compound_cmd = parse_compound_command();
		if (!node->compound_cmd) {
			fatal("Command compound command\n", 1);
		}
	} else {
		return NULL;
	}
	return node;
}

SimpleCommandNode *parse_simple_command() {
	SimpleCommandNode *node = gc_unique(SimpleCommandNode, GC_SUBSHELL);

	node->word = lex_interface(LEX_GET, NULL);
	if (!node->word) {
		fatal("SimpleCommand word", 1);
	}
	return node;
}

CompoundCommandNode *parse_compound_command() {
	CompoundCommandNode *node = gc_unique(CompoundCommandNode, GC_SUBSHELL);
	node->type = IF_CLAUSE;
	node->if_clause = parse_if_clause();
	return node;
}

IfClauseNode *parse_if_clause() {
	IfClauseNode *node = gc_unique(IfClauseNode, GC_SUBSHELL);

	node->condition = parse_compound_list();
	node->body = parse_compound_list();
	if (!node->condition || !node->body) {
		fatal("IfClause any", 1);
	}

	return node;
}


void lexer_tokenize(char *in) {
	lex_interface(LEX_SET, in);
	CompoundListNode *root = parse_compound_list();
	dprintf(2, "tok: %s\n", root->term->and_or->command->simple_cmd->word);
	(void) root;
	//print this shit;
}
