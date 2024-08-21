/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_get_next_token.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 15:53:46 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/21 16:33:51 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "lexer_enum.h"
#include "lexer_struct.h"
#include <stdint.h>
#include <stdio.h>

//Read a char from input
void lexer_read_char(Lexer_p l) {
	if (l->read_position >= l->input_len) {
		l->ch = 0;
	} else {
		l->ch = l->input[l->read_position];
	}
	l->peak_ch = l->ch;
	l->position = l->read_position;
	l->peak_position = l->position;
	l->read_position += 1;
}

void lexer_peak_char(Lexer_p l) {
	l->peak_position += 1;

	if (l->peak_position >= l->input_len) {
		l->peak_ch = '\0';
	} else {
		l->peak_ch = l->input[l->peak_position];
	}
}

void lexer_reverse_peak(Lexer_p l) {
	l->peak_position -= 1;

	if (l->peak_position >= l->input_len) {
		l->peak_ch = '\0';
	} else {
		l->peak_ch = l->input[l->peak_position];
	}
}

void lexer_reset_peak_position(Lexer_p l) {
	l->peak_position = l->position;
}

void lexer_pos_from_peak(Lexer_p l) {
	l->position = l->peak_position;
	l->read_position = l->position + 1;
	l->ch = l->peak_ch;
}

void lexer_peak_from_pos(Lexer_p l) {
	l->peak_position = l->position;
	l->peak_ch = l->ch;
}


//Skips all whitespaces
static void eat_whitespace(Lexer_p l) {
	while (is_whitespace(l->ch)) {
		lexer_read_char(l);
	}
}

//return the type of redirection
static type_of_redirection get_redir_type(Lexer_p l) {
	type_of_redirection type = R_UNKNOWN;
	lexer_peak_char(l);

	if (l->ch == '>') {
		if (l->peak_ch == '>') {
			type = R_APPEND;
			lexer_peak_char(l);
		} else if (l->peak_ch == '&') {
			type = R_DUP_OUT;
			lexer_peak_char(l);
		} else {
			type = R_OUTPUT;
		}
	}

	if (l->ch == '<') {
		if (l->peak_ch == '<') {
			type = R_HERE_DOC;
			lexer_peak_char(l);
		} else if (l->peak_ch == '&') {
			type = R_DUP_IN;
			lexer_peak_char(l);
		} else {
			type = R_INPUT;
		}
	}

	if (l->ch == '&') {
		if (l->peak_ch == '>') {
			lexer_peak_char(l);
			if (l->peak_ch == '>') {
				lexer_peak_char(l);
				type = R_DUP_BOTH_APPEND;
			} else {
				type = R_DUP_BOTH;
			}
		}
	}

	return type;
}

//assert that word is a number followed by a redirection char
bool followed_by_redirection(Lexer_p l) {
	while(is_number(l->peak_ch)) {
		lexer_peak_char(l);
	}
	if (is_redirection_char(l->peak_ch)) return true;
	return false;
}


//assert that the next word is a number
bool word_is_number(Lexer_p l) {
	uint16_t size = 0;
	for (; is_number(l->peak_ch); size += 1) {
		lexer_peak_char(l);
	}
	if (size == 0) return false;
	if (is_whitespace(l->peak_ch) || l->peak_ch == '\0') return true;
	return false;
}

//Returns allocated char *  --> next word in input
static char *get_next_word(Lexer_p l) {
	uint16_t start_position = l->position;
	
	while (l->ch && !is_whitespace(l->ch)) {
		lexer_read_char(l);
	}

	uint16_t end_position = l->position;
	char *word = gc_add(ft_substr(l->input, start_position, end_position - start_position));

	return word;
}

//Identifies the type of expression and returns it
type_of_expression get_expression_type(Lexer_p l) {
	type_of_expression type = EX_ERROR;
	lexer_peak_char(l);
	switch (l->peak_ch) {
		case '(':
			type = EX_ERROR;
			lexer_peak_from_pos(l);
			break;
		case '{':
			type = EX_VARIABLE_EXPANSION;
			lexer_peak_char(l);
			lexer_pos_from_peak(l);
			break;
		default:
			type = EX_WORD;
			break;
	}
	return type;
}


char *get_ctrl_sub_word(Lexer_p l) {
	const uint16_t start = l->position;
	while(l->ch && l->ch != ')') {
		lexer_read_char(l);
	}
	const uint16_t end = l->position;
	lexer_read_char(l);
	char *word = gc_add(ft_substr(l->input, start, end - start));
	return word;
}


char *get_var_exp_name(Lexer_p l) {
	const uint16_t start = l->position;
	while(l->ch && l->ch != '}') {
		lexer_read_char(l);
	}
	const uint16_t end = l->position;
	char *word = gc_add(ft_substr(l->input, start, end - start));
	return word;
}

char *get_error_message(Token *prev, Token *curr, type_of_error type) {
	(void) prev;
	(void) curr;
	switch(type) {
		case ERROR_UNEXPCTED_TOKEN:
			return gc_add(strdup("Fuck you\n"));
		default:
			return gc_add(strdup("Not handled error\n"));
	}
}

void eat_scalar(Lexer_p l, char c) {
	while (l->ch == c) {
		lexer_read_char(l);
	}
}

void read_till_next_scalar(Lexer_p l, char c) {
	while (l->ch != c) {
		lexer_read_char(l);
	}
}

char *get_ctrl_sub_input(Lexer_p l) {
	uint16_t const start = l->position + 1;
	uint16_t par_found = 0;
	uint16_t par_needed = 1;

	while (l->ch && par_found != par_needed) {
		lexer_read_char(l);
		if (l->ch == ')') {
			par_found += 1;
			continue;
        }
		if (l->ch == '(')
			par_needed += 1;
	}

	const uint16_t end = l->position; 
	if (l->ch == ')') lexer_read_char(l);
	return gc_add(ft_substr(l->input, start, end - start));
}

//Extract next token from the input
Token *lexer_get_next_token(Lexer_p l) {
	Token *self = gc_add(ft_calloc(1, sizeof(Token)));
	Token token;

	eat_whitespace(l);

	switch(l->ch) {
		case '\0':
			token.tag = T_END_OF_FILE;
			break;
		
		case '<':
		case '>':
		case '&':
			if (get_redir_type(l) != R_UNKNOWN) {
				lexer_peak_from_pos(l);
				token_redirection_init(&token);
				token.r_type = get_redir_type(l);
				lexer_pos_from_peak(l);
				if (word_is_number(l)) {
					token.fd_postfix = ft_atoi(&l->input[l->position]);
					lexer_pos_from_peak(l);
				} else {
					token.filename =  lexer_get_next_token(l);
				}
			} else {
				lexer_peak_from_pos(l);
				printf("must be job control or &&\n");
			}	
			break;

		case '|':
		case ';':
			printf("Separator !\n");
			break;

		case '\'':
		case '\"':
		case '\\':
			printf("Inhibitors not handled yet\n");
			break;

		case '$':
			token_expression_init(&token);
			token.ex_type = get_expression_type(l);
			if (token.ex_type == EX_ERROR) {
				token_control_group_init(&token);
				lexer_read_char(l);
				token.cs_type = CG_CONTROL_SUBSTITUTION;
			}
			if (token.cs_type == CG_CONTROL_SUBSTITUTION) {
				char *cl_input = get_ctrl_sub_input(l);
				printf("INPUT = %s\n", cl_input);
				Lexer_p cs_l = lexer_init(cl_input);
				token.cs_list = lexer_lex_all(cs_l);
				printf("l->ch %c\n", l->ch);
				if (!is_whitespace(l->ch)) {
					token.cs_postfix = lexer_get_next_token(l);
				}
				break;
			}
			break;

		case 'a' ... 'z':
		case 'A' ... 'Z':
			//handle case of expression postfix an expression
			// printf("WORD = %s\n", &l->input[l->position]);
			token_expression_init(&token);
			token.ex_type = EX_WORD;
			token.ex_infix = get_next_word(l);
			if (!is_whitespace(l->ch)) {
				token.ex_postfix = lexer_get_next_token(l);
			}
			break;

		case '0' ... '9':
			if (followed_by_redirection(l)) {
				token_redirection_init(&token);
				token.fd_prefix = ft_atoi(&l->input[l->position]);
				lexer_pos_from_peak(l);
				token.r_type = get_redir_type(l);
				lexer_pos_from_peak(l);
				if (word_is_number(l)) {
					token.fd_postfix = ft_atoi(&l->input[l->position]);
					lexer_pos_from_peak(l);
				} else {
					token.filename = lexer_get_next_token(l);
				}
			}
			break;

		default:
			printf("Lexing case not handled !\n");
			break;
	}
	*self = token;
	return self;
}

void tokenListToString(TokenList *tl) {
	for (uint16_t i = 0; i < tl->size; i++) {
		tokenToString(tl->t[i], 0);
	}
}

TokenList *lexer_lex_all(Lexer_p l) {
	TokenList *self = token_list_init();
	while (l->ch) {
		token_list_add(self, lexer_get_next_token(l));
	}
	return self;
}
