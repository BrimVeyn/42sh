/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_get_next_token.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 15:53:46 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/19 17:27:58 by bvan-pae         ###   ########.fr       */
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
		l->peak_ch =  '\0';
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

//Return true or false if whitespace
static bool is_whitespace(char c) {
	return (c == '\n' || c == ' ' || c == '\t' || c == '\r' || c == '\v' || c == '\f');
}

//Skips all whitespaces
static void eat_whitespace(Lexer_p l) {
	while (is_whitespace(l->ch)) {
		lexer_read_char(l);
	}
}

//Returns allocated char *  --> next word in input
static char *get_next_word(Lexer_p l) {
	eat_whitespace(l);

	const uint16_t start_position = l->position;
	
	while (l->ch && !is_whitespace(l->ch)) {
		lexer_read_char(l);
	}

	const uint16_t end_position = l->position;
	char *word = ft_substr(l->input, start_position, end_position - start_position);

	return word;
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
	return type;
}

bool is_redirection_char(char c) {
	return (c == '>' || c == '<');
}

bool is_number(char c) {
	return (c >= '0' && c <= '9');
}

bool followed_by_redirection(Lexer_p l) {
	while(is_number(l->peak_ch)) {
		lexer_peak_char(l);
	}
	if (is_redirection_char(l->peak_ch)) {
		return true;
	}
	return false;
}

//Extract next token from the input
Token_or_Error lexer_get_next_token(Lexer_p l) {
	Token token;

	eat_whitespace(l);

	switch(l->ch) {
		case '<':
		case '>':
		case '&':
			//handle case for S_BG
			token.tag = T_REDIRECTION;
			token.r_type = get_redir_type(l);
			token.filename = get_next_word(l);
			break;

		case '|':
		case ';':
			printf("separator \n");
			break;

		case 'a' ... 'z':
		case 'A' ... 'Z':
			printf("salut !\n");
			break;

		case '0' ... '9':
			if (followed_by_redirection(l)) {
				token.fd_prefix = ft_atoi(&l->input[l->position]);
				lexer_pos_from_peak(l);
				token.r_type = get_redir_type(l);
				lexer_pos_from_peak(l);
				token.filename = get_next_word(l);
			}
			break;
	}

	Token_or_Error return_value = {
		.tag = TOKEN,
		.token = token,
	};

	return return_value;
}
