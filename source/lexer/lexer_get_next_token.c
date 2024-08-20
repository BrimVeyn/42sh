/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_get_next_token.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 15:53:46 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/20 18:20:34 by bvan-pae         ###   ########.fr       */
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
	while (is_number(l->peak_ch)) {
		lexer_peak_char(l);
	}
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

type_of_expression get_expression_type(Lexer_p l) {
	type_of_expression type = EX_ERROR;
	lexer_peak_char(l);
	switch (l->peak_ch) {
		case '(':
			type = EX_CONTROL_SUBSTITUTION;
			lexer_peak_char(l);
			lexer_pos_from_peak(l);
			break;
		case '{':
			type = EX_VARIABLE_EXPANSION;
			lexer_peak_char(l);
			lexer_pos_from_peak(l);
			break;
		case '0' ... '9':
		case 'A' ... 'Z':
		case 'a' ... 'z':
			type = EX_WORD;
			break;
		default:
			printf("EX_ERROR ? what do we do boys\n");
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

bool has_child_expr(Lexer_p l) {
	while (l->peak_ch && l->peak_ch != ')') {
		if (l->peak_ch == '$')
			return true;
		lexer_peak_char(l);
	}
	return false;
}

char *get_till_next_expr(Lexer_p l) {
	const uint16_t start = l->position;
	while(l->ch && l->ch != '$') {
		lexer_read_char(l);
	}
	const uint16_t end = l->position;
	char *word = gc_add(ft_substr(l->input, start, end - start));
	return word;
}

//Extract next token from the input
Token_or_Error lexer_get_next_token(Lexer_p l) {
	Token *self = gc_add(ft_calloc(1, sizeof(Token)));
	Token token;

	eat_whitespace(l);

	switch(l->ch) {
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
					Token_or_Error maybe_filename = lexer_get_next_token(l);
					if (maybe_filename.tag == ERROR) {
						printf("An error has occured ?\n");
					} else {
						token.filename = maybe_filename.token;
					}
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

		case '$':
			token_expression_init(&token);
			token.ex_type = get_expression_type(l);
			if (token.ex_type == EX_CONTROL_SUBSTITUTION) {
				if (has_child_expr(l)) {
					token.ex_infix = get_till_next_expr(l);
					Token_or_Error maybe_filename = lexer_get_next_token(l);
					if (maybe_filename.tag == ERROR) {
						printf("An error has occured ?\n");
					} else {
						token.ex_postfix = maybe_filename.token;
					}
				} else {
					token.ex_infix = get_ctrl_sub_word(l);
					// printf("ICI C -> %c\n", l->ch);
					if (l->ch && !is_whitespace(l->ch) && l->ch != ')') {
						Token_or_Error maybe_filename = lexer_get_next_token(l);
						if (maybe_filename.tag == ERROR) {
							printf("An error has occured ?\n");
						} else {
							token.ex_postfix = maybe_filename.token;
						}
					}
				}
			} else if (token.ex_type == EX_VARIABLE_EXPANSION) {
				//to do
				token.ex_infix = get_next_word(l);
			}
			break;

		case 'a' ... 'z':
		case 'A' ... 'Z':
			//handle case of expression postfix an expression
			token_expression_init(&token);
			token.ex_type = get_expression_type(l);
			if (token.ex_type == EX_WORD) {
				if (has_child_expr(l)) {
					token.ex_infix = get_till_next_expr(l);
					Token_or_Error maybe_filename = lexer_get_next_token(l);
					if (maybe_filename.tag == ERROR) {
						printf("An error has occured ?\n");
					} else {
						token.ex_postfix = maybe_filename.token;
					}
				} else {
					token.ex_infix = get_next_word(l);
				}
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
					Token_or_Error maybe_filename = lexer_get_next_token(l);
					if (maybe_filename.tag == ERROR) {
						printf("An error has occured ?\n");
					} else {
						token.filename = maybe_filename.token;
					}
				}
			}
			break;
	}

	*self = token;

	Token_or_Error return_value = {
		.tag = TOKEN,
		.token = self,
	};

	return return_value;
}
