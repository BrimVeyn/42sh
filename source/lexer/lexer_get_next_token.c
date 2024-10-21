/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_get_next_token.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:19:12 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/21 17:34:07 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "utils.h"
#include "libft.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define BITMAP8_SIZE 8
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#define IS_ON(ptr, context) ((*byteptr) & (1 << (int) context)) > 0

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

KeyWord get_keyword(StringStream *cache) {


	static const char *keywords[] = {
		"if", "then", "else", "elif", "fi",
		"case", "esac", "while", "for", "select",
		"until", "do", "done", "in", "function", "time",
		"{", "}", "[", "]"
	};

	for (size_t i = 0; i < ARRAY_SIZE(keywords); i++) {
		if (!ft_strcmp(cache->data, keywords[i])) {
			return i;
		}
	}
	return NOT_KEYWORD;
}


void lexer_tokenize(char *in) {
	da_create(input, StringStream, sizeof(char), GC_SUBSHELL);
	da_create(cache, StringStream, sizeof(char), GC_SUBSHELL);
	ss_push_string(input, in);

	da_create(context_stack, LexerContextList, sizeof(LexerContext), GC_SUBSHELL);
	da_push(context_stack, LEXER_COMMAND);

	static KeyWordsBounds map[] = {
		[IF] = {.end = FI, .bitmap = IF_MAP},
	};

	da_create(token_list, TokenList, sizeof(Token *), GC_SUBSHELL);
	(void)map;

	while (input->size) {
		skip_whitespaces(input);
		//check for subshell or redirection
		//how do we determine if its a keyword or not ?
		//either we're at the start of the line or we didn't find any bin yet ? 
		//cuz if if if if is valid
		parse_word(input, cache);

		KeyWord maybe_keyword = get_keyword(cache);
		(void)maybe_keyword;
		//if its an opening keyword, push to context
		//if it matches the end of context, pop context and push construct ?
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
