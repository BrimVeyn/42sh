/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_get_next_token.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:19:12 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/22 17:36:06 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "utils.h"
#include "libft.h"
#include <readline/chardefs.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define BITMAP8_SIZE 8
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
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

void parse_word(StringStream *input, StringStream *cache) {

	static WordContextBounds map[] = {
		[WORD_WORD] = {.start = "NONE", .end = " \t\n", .bitmap = WORD_MAP},
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
			}
			continue;
		}

		da_push(cache, da_pop_front(input));
	}

	if (context_stack->size != 0 && da_peak_back(context_stack) != WORD_WORD) {
		dprintf(2, "SYNTAX ERROR UNCLOSED SHIT\n");
	}
}

void skip_whitespaces(StringStream *input) {
	while (ft_strchr(" \n\t", input->data[0])) {
		da_pop_front(input);
	}
}

Lexem get_lexem(StringStream *cache, LexemInfos *context) {

	if (!*cache->data) {
		return CEOF;
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
				dprintf(2, "FATAL LEXEM ROCOGNITION\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	if (context->lexem == COMMAND) {
		return UNTAGGED;
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

char *LexemStr(Lexem lexem) {

	static const struct {
		Lexem lexem;
		char  *value;
	} litterals[] = {
		{LNONE, "stack_bottom"}, {IF, "if"}, {THEN, "then"}, {ELSE, "else"},
	    {ELIF, "elif"}, {FI, "fi"}, {CASE, "case"},
		{ESAC, "esac"}, {WHILE, "while"}, {FOR, "for"},
		{SELECT, "select"}, {UNTIL, "until"}, {DO, "do"},
		{DONE, "done"}, {IN, "in"}, {FUNCTION, "function"},
		{TIME, "time"}, {L_CURLY_BRACKET, "{"}, {R_CURLY_BRACKET, "}"},
		{L_SQUARE_BRACKET, "["}, {R_SQUARE_BRACKET, "]"},
		{SEMI_COLUMN, ";"}, {COMMAND, "CMD"}, {NEWLINE, "\n"},
		{CEOF, "EOF"}, {UNTAGGED, "bin/arg"},
	};

	for (size_t i = 0; i < ARRAY_SIZE(litterals); i++) {
		if (lexem == litterals[i].lexem) {
			return litterals[i].value;
		}
	}
	return "INVALID";
}

#include "colors.h"

void printContextStack(LexemContextStack *stack) {
	dprintf(2, C_LIGHT_RED"--------Lexem Stack--------"C_RESET"\n");
	for (size_t i = 0; i < stack->size; i++) {
		dprintf(2, "[%zu]: %s\n",i, LexemStr(stack->data[i]->lexem));
	}
	dprintf(2, C_LIGHT_RED"---------------------------"C_RESET"\n");
}

LexemInfos *get_lexem_infos(LexemInfos *map, size_t map_size, Lexem lexem) {
	for (size_t i = 0; i < map_size; i++) {
		if (map[i].lexem == lexem) {
			LexemInfos *ptr = gc_unique(LexemInfos, GC_SUBSHELL);
			ft_memcpy(ptr, &map[i], sizeof(LexemInfos));
			return ptr;
		}
	}
	return NULL;
}

void printLexemState(LexemInfos *infos) {
	if (!infos) 
		return ;
	char buffer[MAX_WORD_LEN] = {0};
	ft_sprintf(buffer, C_BRIGHT_CYAN"%s"C_RESET": "C_BRIGHT_YELLOW, LexemStr(infos->lexem));
	for (size_t i = 0; i < 31; i++) {
		if ((infos->ctx_mandatory & (1 << i)) > 0) {
			ft_sprintf(buffer, "%s ", LexemStr(1 << i));
		}
	}
	ft_sprintf(buffer, C_RESET"\n");
	ft_putstr_fd(buffer, 2);
}

void printStackStates(LexemContextStack *stack) {
	dprintf(2, C_LIGHT_YELLOW"------STATES------"C_RESET"\n");
	for (size_t i = 0; i < stack->size; i++) {
		printLexemState(stack->data[i]);
	}
	dprintf(2, C_LIGHT_YELLOW"------------------"C_RESET"\n");
}

typedef struct {
	TokenList **conditions;
	TokenList **bodys;
	TokenList *else_body;
} IfNode;

void lexer_tokenize(char *in) {
	da_create(input, StringStream, sizeof(char), GC_SUBSHELL);
	da_create(cache, StringStream, sizeof(char), GC_SUBSHELL);
	ss_push_string(input, in);


	static LexemInfos map[] = {
		{.lexem = LNONE,   .ctx_end = 0,                    .ctx_allowed = (COMMAND | IF),        .ctx_mandatory = 0,                .parent = 0},
		{.lexem = IF,      .ctx_end = (FI),                 .ctx_allowed = (COMMAND | THEN | IF), .ctx_mandatory = (COMMAND | THEN), .parent = 0},
		{.lexem = ELIF,	   .ctx_end = (FI),					.ctx_allowed = (COMMAND | THEN | IF), .ctx_mandatory = (COMMAND | THEN), .parent = IF},
		{.lexem = THEN,    .ctx_end = (ELSE | ELIF | FI),   .ctx_allowed = (COMMAND | IF),        .ctx_mandatory = (COMMAND),        .parent = IF},
		{.lexem = ELSE,    .ctx_end = (FI),                 .ctx_allowed = (COMMAND | IF),        .ctx_mandatory = (COMMAND),        .parent = IF},
		{.lexem = COMMAND, .ctx_end = (SEMI_COLUMN | CEOF), .ctx_allowed = 0,			          .ctx_mandatory = 0,                .parent = 0},
		{.lexem = SEMI_COLUMN, .ctx_end = 0,                .ctx_allowed = 0,                     .ctx_mandatory = 0,                .parent = 0},
	};

	da_create(token_list, TokenList, sizeof(Token *), GC_SUBSHELL);
	da_create(context_stack, LexemContextStack, sizeof(LexemInfos *), GC_SUBSHELL);

	Lexem rd_lexem = LNONE;
	LexemInfos *stack_bottom_infos = get_lexem_infos(map, ARRAY_SIZE(map), LNONE);
	da_push(context_stack, stack_bottom_infos);

	while (rd_lexem != CEOF) {
		da_clear(cache);
		skip_whitespaces(input);
		parse_word(input, cache);

		printStackStates(context_stack);
		rd_lexem = get_lexem(cache, da_peak_back(context_stack));
		dprintf(2, "result: |%s|, value: %s\n", cache->data, LexemStr(rd_lexem));
		if (rd_lexem == UNTAGGED)
			continue;
		LexemInfos *rd_lexem_infos = get_lexem_infos(map, ARRAY_SIZE(map), rd_lexem);
		// printContextStack(context_stack);
		if (!context_stack->size) {
			if (rd_lexem == CEOF)
				break;
			dprintf(2, "RootContext: "C_BRIGHT_YELLOW"%s"C_RESET"\n", LexemStr(rd_lexem)); 
			da_push(context_stack, rd_lexem_infos);
			continue;
		} else {
			if (is_valid_context(da_peak_back(context_stack), rd_lexem, CONTEXT_NEW)) {
				dprintf(2, "NestedContext: "C_BRIGHT_YELLOW"%s"C_RESET"\n", LexemStr(rd_lexem));
				LexemInfos *top_context = da_peak_back(context_stack);
				if ((top_context->ctx_mandatory & rd_lexem) > 0) {
					top_context->ctx_mandatory ^= rd_lexem;
				}
				da_push(context_stack, rd_lexem_infos);
				continue;
			} 
		}

		if (is_valid_context(da_peak_back(context_stack), rd_lexem, CONTEXT_END)) {
			dprintf(2, "ContextEnd: "C_BRIGHT_YELLOW"%s"C_RESET"\n", LexemStr(da_peak_back(context_stack)->lexem));
			LexemInfos *top_context = da_peak_back(context_stack);
			Lexem parent = top_context->parent; 
			if (top_context->ctx_mandatory == 0) {
				da_pop(context_stack);
			}
			if (parent) {
				da_pop(context_stack);
				LexemInfos *nested_parent = da_peak_back(context_stack);
				//TODO: add a check to actually prove parent is the target
				while (nested_parent != NULL && nested_parent->parent) {
					da_pop(context_stack);
					nested_parent = da_peak_back(context_stack);
				}
			}
			continue;
		}
	}

	if (context_stack->size > 1) {
		printContextStack(context_stack);
		dprintf(2, C_DARK_RED"Unexpected token near somewhere !"C_RESET"\n");
	}
}

TokenList *lexer_lex_all(char *input) {
	Lexer_p l = lexer_init(input);
	da_create(self, TokenList, sizeof(Token *), GC_SUBSHELL);
	while (true) {
		Token *tmp = lexer_get_next_token(l);
		da_push(self, tmp);
		if (tmp->tag == T_SEPARATOR && tmp->s_type == S_EOF) break;
	}
	return self;
}

void init_token_from_tag(Token *token) {
	switch (token->tag) {
		case T_REDIRECTION:
			token_redirection_init(token);
			break;
		case T_WORD:
			token_word_init(token);
		default:
			return;
	}
}

//Extract next token from the input
Token *lexer_get_next_token(Lexer_p l) {
	Token *token = gc(GC_ADD, ft_calloc(1, sizeof(Token)), GC_SUBSHELL);

	eat_whitespace(l);

	token->tag = get_token_tag(l);
	token->e = ERROR_NONE;

	init_token_from_tag(token);

	switch(token->tag) {
		case T_SEPARATOR:
			token->s_type = get_separator_type(l); 
			break;
		case T_REDIRECTION:
			token->r_type = get_redirection_type(l);
			eat_whitespace(l);
			token->r_postfix = lexer_get_next_token(l);
			if (token->r_postfix->tag != T_WORD) {
				token->e = ERROR_UNEXPECTED_TOKEN;
			}
			break;
		case T_WORD:
			token->w_infix = get_word(l);
			if (is_number(token->w_infix) && !is_whitespace(l->ch) && is_fdable_redirection(l)) {
				token->w_postfix = lexer_get_next_token(l);
			} else break;
			if (token->w_postfix->tag == T_REDIRECTION && 
				token->w_postfix->r_postfix->tag == T_NONE) {
				token->e = ERROR_UNEXPECTED_TOKEN;
			}
			break;
		default:
			printf("Default case of get_next_token !\n");
			exit(EXIT_FAILURE);
			break;
	}
	return token;
}
