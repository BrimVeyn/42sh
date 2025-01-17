/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arithmetic_lexer.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 11:46:55 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/29 23:37:11 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "arithmetic.h"
#include "ft_regex.h"
#include "lexer.h"
#include "libft.h"
#include "parser.h"
#include "utils.h"
#include "dynamic_arrays.h"

#include <stdio.h>
#include <unistd.h>

arithemtic_token_tag get_atoken_tag(Lexer * l) {
	if (regex_match("^[_0-9a-zA-Z]", &l->input[l->position]).re_start != -1) {
		return A_OPERAND;
	} else {
		return A_OPERATOR;
	}
}

arithmetic_operators get_operator_tag(Lexer * l) {
	const char *input_ptr = &l->input[l->position];
	static const struct {
		const char *op_str;
		const arithmetic_operators op_tab;
	} multi_char_op[] = {
		{"++", O_PREF_INCR},	{"--", O_PREF_DECR},
		{"++", O_PREF_INCR},	{"--", O_PREF_DECR},
		{"<=", O_LE},	{">=", O_GE},
		{"==", O_EQUAL},{"!=", O_DIFFERENT},
		{"&&", O_AND},	{"||", O_OR},
	};

	for (size_t i = 0; i < sizeof(multi_char_op) / sizeof(multi_char_op[0]); i++) {
		if (!ft_strncmp(input_ptr, multi_char_op[i].op_str, 2)) {
			lexer_read_x_char(l, 2);
			return multi_char_op[i].op_tab;
		}
	}
    switch (l->ch) {
        case '(': lexer_read_char(l); return O_POPEN;
        case ')': lexer_read_char(l); return O_PCLOSE;
        case '+': lexer_read_char(l); return O_PLUS;
        case '-': lexer_read_char(l); return O_MINUS;
        case '*': lexer_read_char(l); return O_MULT;
        case '/': lexer_read_char(l); return O_DIVIDE;
        case '%': lexer_read_char(l); return O_MODULO;
        case '<': lexer_read_char(l); return O_LT;
        case '>': lexer_read_char(l); return O_GT;
        default:  return O_ERROR;
    }
}

bool is_operator_char(char c) {
	return (ft_strchr("+-*/%<>=!|&()", c));
}

bool fill_operand(AToken *self, Lexer * l) {
	const int start = l->position;
	while (ft_isalnum(l->ch) || l->ch == '_') {
		lexer_read_char(l);
	}
	const int end = l->position;
	char *var = ft_substr(l->input, start, end - start);
	if (!(!l->ch || is_whitespace(l->ch) || is_operator_char(l->ch))) {
		free(var);
		return false;
	}
	if (!is_number(var) && regex_match("^[_a-zA-Z][a-zA-Z1-9_]*", var).re_start == -1) {
		free(var);
		return false;
	}
	if (is_number(var)) {
		self->operand_tag = VALUE;
		self->litteral = ft_atol(var); //make it signed long
	} else {
		self->operand_tag = VARIABLE;
		self->variable = var;
	}
	gc(GC_ADD, var, GC_SUBSHELL);
	return true;
}

AToken *lexer_get_next_atoken(Lexer * l) {
	AToken *self = gc(GC_ADD, ft_calloc(1, sizeof(AToken)), GC_SUBSHELL);

	eat_whitespace(l);
	if (!l->ch) {
		self->tag = A_EOF;
		return self;
	}

	self->tag = get_atoken_tag(l);

	switch (self->tag) {
		case A_OPERATOR:
			//catch unknown token and return an error;
			self->operator = get_operator_tag(l);
			if (self->operator == O_ERROR) {
				char buffer[2] = {l->ch, 0};
				ARITHMETIC_SYNTAX_ERROR(buffer);
				g_exitno = 1;
				return NULL;
			}
			break;
		case A_OPERAND:
			if (!fill_operand(self, l)) {
				char buffer[2] = {l->ch, 0};
				ARITHMETIC_SYNTAX_ERROR(buffer);
				g_exitno = 1;
				return NULL;
			}
			break;
		default:
			ft_dprintf(2, "Fatal arithmetic expansion lexer\n");
			exit(EXIT_FAILURE);
	}
	return self;
}

void free_atoken_stack(ATokenStack *list) {
	for (size_t i = 0; i < list->size; i++) {
		gc(GC_FREE, list->data[i], GC_SUBSHELL);
	}
	gc(GC_FREE, list, GC_SUBSHELL);
}

ATokenStack *lexer_arithmetic_exp_lex_all(Lexer * lexer) {
	da_create(self, ATokenStack, sizeof(AToken *), GC_SUBSHELL);

	while (lexer->ch) {
		AToken *tmp = lexer_get_next_atoken(lexer);
		if (!tmp) {
			gc(GC_FREE, lexer, GC_SUBSHELL);
			free_atoken_stack(self);
			return NULL;
		} else if (tmp->tag == A_EOF) return self;
		da_push(self, tmp);
	}
	return self;
}
