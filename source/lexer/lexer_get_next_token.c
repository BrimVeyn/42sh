/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_get_next_token.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 15:53:46 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/15 17:31:32 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "lexer_enum.h"
#include <stdint.h>

//Read a char from input
void lexer_read_char(Lexer_p l) {
	if (l->read_position >= strlen(l->input)) {
		l->ch = 0;
	} else {
		l->ch = l->input[l->read_position];
	}

	l->position = l->read_position;
	l->read_position += 1;
}

char lexer_peak_char(Lexer_p l) {
	if (l->read_position >= strlen(l->input)) {
		return '\0';
	} else {
		return l->input[l->read_position];
	}
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
	uint16_t start_position = l->position;
	
	while (!is_whitespace(l->ch)) {
		lexer_read_char(l);
	}

	uint16_t end_position = l->position;
	char *word = ft_substr(l->input, start_position, end_position - start_position);

	return word;
}

//return the type of redirection
static enum type_of_redirection get_redir_type(Lexer_p l) {
	if (l->ch == '<') {
		if (lexer_peak_char(l) != '<') {

		} else if (lexer_peak_char(l) == '<') {

		} else {

		}
		return R_INPUT_FILE;
	} else {

		if (lexer_peak_char(l) != '>') {

		} else if (lexer_peak_char(l) == '>') {

		} else {

		}
		return R_APPEND_FILE;
	}
}

//Extract next token from the input
Token lexer_get_next_token(Lexer_p l) {
	Token token;

	eat_whitespace(l);

	switch(l->ch) {
		case '<':
		case '>':
			token.tag = T_REDIRECTION;
			token.redir.type = get_redir_type(l);
			token.redir.filename = get_next_word(l);
			break;

		case '&':
		case '|':
		case ';':
			printf("separator \n");
			break;

		case 'a' ... 'z':
		case 'A' ... 'Z':
			printf("salut !\n");
			break;
	}

	return token;
}
