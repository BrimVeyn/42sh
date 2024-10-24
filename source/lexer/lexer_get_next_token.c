/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_get_next_token.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:19:12 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/24 16:55:27 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "parser.h"
#include "utils.h"
#include "libft.h"
#include "debug.h"
#include "colors.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define BITMAP8_SIZE 8
#define IS_ON(ptr, context) ((*ptr) & (1 << (int) context)) > 0

WordContext get_context(const StringStream *input, WordContextBounds *map, const WordContext context) {
	const unsigned char *byteptr = (unsigned char *)&map[context].bitmap;

	if (IS_ON(byteptr, WORD_ARITHMETIC) && !ft_strncmp(input->data, map[WORD_ARITHMETIC].start, 3)) {
		dprintf(2, "yes arithmetic\n");
		return WORD_ARITHMETIC;
	}
	if (IS_ON(byteptr, WORD_CMD_SUB) && !ft_strncmp(input->data, map[WORD_CMD_SUB].start, 2)) {
		dprintf(2, "yes cmd_sub\n");
		return WORD_CMD_SUB;
	}
	if (IS_ON(byteptr, WORD_PARAM) && !ft_strncmp(input->data, map[WORD_PARAM].start, 2)) {
		dprintf(2, "yes param\n");
		return WORD_PARAM;
	}
	if (IS_ON(byteptr, WORD_SUBSHELL) && !ft_strncmp(input->data, map[WORD_SUBSHELL].start, 1)) {
		dprintf(2, "yes subshell\n");
		return WORD_SUBSHELL;
	}
	if (IS_ON(byteptr, WORD_ARITHMETIC_PAREN) && !ft_strncmp(input->data, map[WORD_ARITHMETIC_PAREN].start, 1)) {
		dprintf(2, "yes arithmetic paren\n");
		return WORD_ARITHMETIC_PAREN;
	}
	if (IS_ON(byteptr, WORD_SINGLE_QUOTE) && !ft_strncmp(input->data, map[WORD_SINGLE_QUOTE].start, 1)) {
		dprintf(2, "yes Single quote\n");
		return WORD_SINGLE_QUOTE;
	}
	if (IS_ON(byteptr, WORD_DOUBLE_QUOTE) && !ft_strncmp(input->data, map[WORD_DOUBLE_QUOTE].start, 1)) {
		dprintf(2, "yes double quote\n");
		return WORD_DOUBLE_QUOTE;
	}
	return NONE;
}

WordContext get_end_of_context(const StringStream *input, WordContextBounds *map, const WordContext context) {
	if (context == WORD_WORD && ft_strchr(map[context].end, input->data[0])) {
		return context;
	} else if (context == WORD_WORD) {
		return NONE;
	}

	if (!ft_strncmp(input->data, map[context].end, ft_strlen(map[context].end))) {
		return context;
	} else {
		return NONE;
	}
}

void parse_word(StringStream *input, StringStream *cache, size_t *line, size_t *column) {

	static WordContextBounds map[] = {
		[WORD_WORD] = {.start = "NONE", .end = " \t\n;", .bitmap = WORD_MAP},
		[WORD_CMD_SUB] = {"$(", ")", CMD_SUB_MAP},
		[WORD_PARAM] = { "${", "}" , PARAM_MAP},
		[WORD_SUBSHELL] = { "(", ")", SUBSHELL_MAP},
		[WORD_ARITHMETIC] = {"$((", "))" , ARITHMETIC_MAP},
		[WORD_ARITHMETIC_PAREN] = { "(", ")" , ARITHMETIC_PAREN_MAP},
		[WORD_SINGLE_QUOTE] = {"'", "'", SINGLE_QUOTE_MAP},
		[WORD_DOUBLE_QUOTE] = {"\"", "\"", DOUBLE_QUOTE_MAP},
	};

	da_create(context_stack, WordContextList, sizeof(WordContext), GC_SUBSHELL);
	da_push(context_stack, WORD_WORD);

	while (input->size) {
		WordContext maybe_new_context = get_context(input, map, da_peak_back(context_stack));

		if (maybe_new_context != NONE) {
			da_push(context_stack, maybe_new_context);
			for (size_t i = 0; i < ft_strlen(map[maybe_new_context].start); i++) {
				da_push(cache, da_pop_front(input));
				(*column)++;
			}
			continue;
		}

		WordContext mayber_end_of_context = get_end_of_context(input, map, da_peak_back(context_stack));

		if (mayber_end_of_context != NONE) {
			da_pop(context_stack);
			if (!context_stack->size) {
				break;
			}
			for (size_t i = 0; i < ft_strlen(map[mayber_end_of_context].end); i++) {
				da_push(cache, da_pop_front(input));
				(*column)++;
			}
			continue;
		}

		char c = da_pop_front(input);
		if (c == '\n') {
			(*line)++;
			(*column) = 0;
		} else {
			(*column)++;
		}
		da_push(cache, c);
	}

	if (!*cache->data) {
		if (!ft_strncmp(input->data, ";", 1)) {
			da_push(cache, da_pop_front(input));
			(*column)++;
		}
	}

	if (context_stack->size != 0 && da_peak_back(context_stack) != WORD_WORD) {
		//TODO: gcc error 
		dprintf(2, "SYNTAX ERROR UNCLOSED SHIT\n");
	}
}

void skip_whitespaces(StringStream *input, size_t *line, size_t *column) {
	if (input->data[0] == '\n') {
		da_pop_front(input);
		(*line)++;
		(*column) = 0;
	}
	while (ft_strchr(" \n\t", input->data[0])) {
		da_pop_front(input);
		(*column)++;
	}
}

Lexem get_lexem(StringStream *cache, LexemInfos *context, bool *error) {

	if (!*cache->data) {
		return ENDOFFILE;
	}

	static const struct {
		Lexem lexem;
		char  *value;
	} litterals[] = {
		{IF, "if"}, {THEN, "then"}, {ELSE, "else"},
	    {ELIF, "elif"}, {FI, "fi"}, {CASE, "case"},
		{ESAC, "esac"}, {WHILE, "while"}, {FOR, "for"},
		{SELECT, "select"}, {UNTIL, "until"}, {DO, "do"},
		{DONE, "done"}, {IN, "in"}, {FUNCTION, "function"},
		{TIME, "time"}, {L_CURLY_BRACKET, "{"}, {R_CURLY_BRACKET, "}"},
		{L_SQUARE_BRACKET, "["}, {R_SQUARE_BRACKET, "]"},
		{SEMI_COLUMN, ";"},
	};

	for (size_t i = 0; i < ARRAY_SIZE(litterals); i++) {
		if (!ft_strcmp(cache->data, litterals[i].value)) {
			if ((context->ctx_allowed & litterals[i].lexem) > 0 || (context->ctx_end & litterals[i].lexem) > 0) {
				return litterals[i].lexem;
			} else {
				(*error) = true;
			}
		}
	}
	if (context->lexem == COMMAND) {
		return ARGS;
	}
	return COMMAND;
}

#define CONTEXT_END true
#define CONTEXT_NEW false

bool is_valid_context(LexemInfos *current_context, Lexem next_lexem, bool where) {
	if (where == CONTEXT_END && (current_context->ctx_end & next_lexem) > 0) {
		return true;
	}
	if (where == CONTEXT_NEW && (current_context->ctx_allowed & next_lexem) > 0) {
		return true;
	}
	return false;
}

LexemInfos *get_lexem_infos(LexemInfos *map, size_t map_size, Lexem lexem, size_t *line, size_t *column) {
	for (size_t i = 0; i < map_size; i++) {
		if (map[i].lexem == lexem) {
			LexemInfos *ptr = gc_unique(LexemInfos, GC_SUBSHELL);
			ft_memcpy(ptr, &map[i], sizeof(LexemInfos));
			ptr->line = (*line);
			ptr->column = (*column);
			return ptr;
		}
	}
	return NULL;
}

void mandatoryAddBack(MandatoryLexemList **lst, MandatoryLexemList *new_value) {
	MandatoryLexemList	*temp;

	if (*lst == NULL)
		*lst = new_value;
	else {
		temp = *lst;
		while (temp->next)
			temp = temp->next;
		temp->next = new_value;
	}
}

void mandatoryPopFront(MandatoryLexemList **lst) {
	if (!(*lst)) {
		return ;
	} else {
		(*lst) = (*lst)->next;
	}
}

MandatoryLexemList *mandatoryNew(unsigned int value) {
	MandatoryLexemList *self = gc_unique(MandatoryLexemList, GC_SUBSHELL);
	self->value = value;
	return self;
}

void end_context(LexemContextStack *context_stack, Lexem rd_lexem) {
	LexemInfos *top_context = da_peak_back(context_stack);
	Lexem parent = top_context->parent;
	if (!top_context->ctx_mandatory) {
		da_pop(context_stack);

		if (parent != 0) {
			LexemInfos *parent_context = da_peak_back(context_stack);
			bool has_parent = parent;
			if (parent_context->ctx_mandatory && (parent_context->ctx_mandatory->value & rd_lexem) > 0) {
				mandatoryPopFront(&parent_context->ctx_mandatory);
			}
			while (has_parent && parent_context && !parent_context->ctx_mandatory) {
				da_pop(context_stack);

				has_parent = parent_context->parent;
				parent_context = da_peak_back(context_stack);
				if (parent_context->ctx_mandatory && (parent_context->ctx_mandatory->value & rd_lexem) > 0) {
					mandatoryPopFront(&parent_context->ctx_mandatory);
				}
			}
		}
	}
}

SeparatorType identify_separator(char *litteral) {
	const char *map[] = {
		[SEP_AND] = "&&",
		[SEP_OR] = "||",
		[SEP_SEMI] = ";",
		[SEP_BG] = "&",
		[SEP_NEWLINE] = "\n",
		[SEP_PIPE] = "|",
	};
	for (size_t i = 0; ARRAY_SIZE(map); i++) {
		if (!ft_strcmp(litteral, map[i])) {
			return i;
		}
	}
	return 0;
}

Expr *expr_init(Lexem rd_lexem, char *litteral) {
	switch (rd_lexem) {
		case COMMAND:
		case ARGS: {
				Expr *self = gc_unique(Expr, GC_SUBSHELL);
				self->type = EXPR_WORD;
				self->word = gc_unique(Word, GC_SUBSHELL);
				self->word->litteral = gc(GC_ADD, ft_strdup(litteral), GC_SUBSHELL);
				return self;
			}
		case SEMI_COLUMN: 
		case NEWLINE: {
				Expr *self = gc_unique(Expr, GC_SUBSHELL);
				self->type = EXPR_SEP;
				self->sep = gc_unique(Separator, GC_SUBSHELL);
				self->sep->type = SEP_SEMI;
				self->sep->litteral = gc(GC_ADD, ft_strdup(litteral), GC_SUBSHELL);
				return self;
			}
		case EXPR_IF: {
				Expr *self = gc_unique(Expr, GC_SUBSHELL);
				self->type = EXPR_IF;
				self->ifstruct = gc_unique(IfStruct, GC_SUBSHELL);
				da_create(tmp, ExprArrayArray, sizeof(ExprArray *), GC_SUBSHELL);
				self->ifstruct->bodies = tmp;
				da_create(tmp2, ExprArrayArray, sizeof(ExprArray *), GC_SUBSHELL);
				self->ifstruct->conditions = tmp2;
				da_create(tmp3, ExprArray, sizeof(Expr *), GC_SUBSHELL);
				self->ifstruct->else_body = tmp3;
				return self;
			}
		default:
			return NULL;
	}
}

size_t find_rightmost_if(ExprArray *pending) {
	for (size_t i = pending->size - 1; i < pending->size; i--) {
		if (pending->data[i]->type == EXPR_IF) {
			return i;
		}
	}
	return 0;
}

void fill_condition(ExprArray *condition, ExprArray *pending, size_t idx) {
	while (pending->size != idx + 1) {
		da_push_front(condition, da_pop(pending));
	}
}


MandatoryLexemList *getMandatoryList(Lexem lexem) {
	switch (lexem) {
		case ELIF:
		case IF: {
			MandatoryLexemList *head = NULL;
			mandatoryAddBack(&head, mandatoryNew(COMMAND | IF));
			mandatoryAddBack(&head, mandatoryNew(THEN));
			return head;
		}
		case ELSE:
		case THEN: {
			MandatoryLexemList *head = NULL;
			mandatoryAddBack(&head, mandatoryNew(COMMAND | IF));
			return head;
		}
		default: return NULL;
	}
}

#define LSEP (SEMI_COLUMN | NEWLINE | ENDOFFILE)

void lexer_tokenize(char *in) {

	LexemInfos map[] = {
		{LNONE,   .ctx_end = 0,	.ctx_allowed = (COMMAND | IF), .ctx_mandatory = 0,	.parent = 0, .line = 0, .column = 0},
		{IF,		(FI),	(COMMAND | ARGS | THEN | IF),				getMandatoryList(IF),	0, 0, 0},
		{ELIF,		(FI),	(COMMAND | ARGS | THEN | IF), 				getMandatoryList(ELIF), IF, 0, 0},
		{THEN,		(FI),	(COMMAND | ARGS | IF | FI | ELIF | ELSE),   getMandatoryList(THEN),	IF, 0, 0},
		{ELSE,		(FI),	(COMMAND | ARGS | IF | FI),					getMandatoryList(ELSE), IF, 0, 0},
		{FI,		(LSEP),	0,											getMandatoryList(FI),   IF, 0, 0},
		{COMMAND,	(LSEP),	(ARGS),										getMandatoryList(COMMAND),	0, 0, 0},
		{SEMI_COLUMN, 0,	0,											getMandatoryList(SEMI_COLUMN),  0, 0, 0},
		{ARGS,		(ARGS),	0,											getMandatoryList(ARGS),			0, 0, 0},
	};

	da_create(input, StringStream, sizeof(char), GC_SUBSHELL);
	da_create(cache, StringStream, sizeof(char), GC_SUBSHELL);
	ss_push_string(input, in);

	da_create(token_list, TokenList, sizeof(Token *), GC_SUBSHELL);

	size_t line = 0, column = 0;

	Lexem rd_lexem = LNONE;
	LexemInfos *stack_bottom_infos = get_lexem_infos(map, ARRAY_SIZE(map), LNONE, &line, &column);

	da_create(context_stack, LexemContextStack, sizeof(LexemInfos *), GC_SUBSHELL);
	da_push(context_stack, stack_bottom_infos);

	bool error = false;

	da_create(result_array, ExprArray, sizeof(Expr *), GC_SUBSHELL);
	da_create(pending_array, ExprArray, sizeof(Expr *), GC_SUBSHELL);

	while (rd_lexem != ENDOFFILE) {
		da_clear(cache);
		skip_whitespaces(input, &line, &column);

		parse_word(input, cache, &line, &column);

		rd_lexem = get_lexem(cache, da_peak_back(context_stack), &error);

		dprintf(2, "result: |%s|, value: %s | %d\n", cache->data, LexemStr(rd_lexem), rd_lexem);
		LexemInfos *rd_lexem_infos = get_lexem_infos(map, ARRAY_SIZE(map), rd_lexem, &line, &column);
		if (error) {
			da_push(context_stack, rd_lexem_infos);
			break;
		}

		LexemInfos *top_context = da_peak_back(context_stack);
		if (top_context->ctx_mandatory && (top_context->ctx_mandatory->value & rd_lexem) > 0) {
			mandatoryPopFront(&top_context->ctx_mandatory);
		}

		Expr *rd_expr = expr_init(rd_lexem, cache->data);
		if (rd_expr) {
			da_push(pending_array, rd_expr);
		}


		if (is_valid_context(top_context, rd_lexem, CONTEXT_NEW)) {
			if (rd_lexem == THEN) {
				da_create(condition, ExprArray, sizeof(Expr *), GC_SUBSHELL);
				size_t if_idx = find_rightmost_if(pending_array);
				fill_condition(condition, pending_array, if_idx);
				da_push(pending_array->data[if_idx]->ifstruct->conditions, condition);
			}
			if (rd_lexem == ELIF || rd_lexem == FI || rd_lexem == ELSE) {
				da_create(condition, ExprArray, sizeof(Expr *), GC_SUBSHELL);
				size_t if_idx = find_rightmost_if(pending_array);
				fill_condition(condition, pending_array, if_idx);
				da_push(pending_array->data[if_idx]->ifstruct->bodies, condition);
			}
			da_push(context_stack, rd_lexem_infos);
		} 

		printStackStates(context_stack, C_ROSE);
		
		if (is_valid_context(da_peak_back(context_stack), rd_lexem, CONTEXT_END)) {
			end_context(context_stack, rd_lexem);
		}
	}
	
	da_print(pending_array);

	printStackStates(context_stack, C_LIGHT_BROWN);
	if (context_stack->size > 1 || error) {
		LexemInfos *error_context = da_pop(context_stack);
		char *line_content = get_line_x(in, error_context->line);
		char buffer[MAX_WORD_LEN] = {0};
		size_t base = ft_snprintf(buffer, MAX_WORD_LEN, C_BOLD"script.42sh:%ld:%ld "C_BRIGHT_RED"error: "C_RESET"%s\n", error_context->line, error_context->column, "unexpected token");
		size_t offset = ft_snprintf(buffer, MAX_WORD_LEN, "  %ld  |    ", error_context->line);
		ft_snprintf(buffer, MAX_WORD_LEN, "%s\n", line_content);
		for (size_t i = 0; i < (offset - base) + error_context->column - 1; i++) {
			ft_snprintf(buffer, MAX_WORD_LEN, " ");
		}
		ft_snprintf(buffer, MAX_WORD_LEN, C_RED"^"C_RESET"\n");
		ft_putstr_fd(buffer, 2);
	}
	da_print(result_array);
}
