/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arithmetic_expansion.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 15:46:07 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/30 00:06:57 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "arithmetic.h"
#include "lexer.h"
#include "libft.h"
#include "parser.h"
#include "utils.h"
#include "assert.h"
#include "dynamic_arrays.h"

#include <stdio.h>

static operator_precedence get_precedence(const AToken *token) {
	static const struct {
		arithmetic_operators OP;
		operator_precedence P;
	} precedence_table[] = {
		{O_POST_DECR, P6}, {O_POST_INCR, P6},
		{O_PREF_DECR, P5}, {O_PREF_INCR, P5},
		{O_MULT, P4}, {O_DIVIDE, P4}, {O_MODULO, P4},
		{O_PLUS, P3}, {O_MINUS, P3},
		{O_LE, P2}, {O_GE, P2}, {O_LT, P2}, {O_GT, P2},
		{O_EQUAL, P1}, {O_DIFFERENT, P1},
		{O_AND, P0}, {O_OR, P0},
		{O_PCLOSE, PP}, {O_POPEN, PP},
	};

	for (size_t i = 0; i < ARRAY_SIZE(precedence_table); i++) {
		if (token->operator == precedence_table[i].OP)
			return precedence_table[i].P;
	}
	return P0;
}

static bool has_higher_prec(const ATokenStack *operator, const AToken *current) {
	if (!operator->size) 
		return false;
	const AToken *stack_top = operator->data[operator->size - 1];
	return (get_precedence(stack_top) >= get_precedence(current));
}

ATokenStack *tokens_to_rpn(ATokenStack *list) {
	da_create(output, ATokenStack, sizeof(AToken *), GC_SUBSHELL);
	da_create(operator, ATokenStack, sizeof(AToken *), GC_SUBSHELL);
	
	for (int i = 0; i < list->size; i++) {
		AToken *current = list->data[i];
		if (is_lparen(current)) {
			da_push(operator, current);
		} else if (is_rparen(current)) {
			while (operator->size) {
				AToken *popped = da_pop(operator);
				if (is_lparen(popped))
					break;
				da_push(output, popped);
            }
		} else if (!is_aoperator(current)) {
			da_push(output, current);
		} else {
			while (has_higher_prec(operator, current)) {
				da_push(output, da_pop(operator));
			}
			da_push(operator, current);
		}
	}
	while (operator->size) {
		da_push(output, da_pop(operator));
	}
	return output;
}

static void copy_till_end_of_nested(const char *ref, char *buffer, int *i, int *j, const int n) {
	buffer[(*j)++] = '(';
	(*i) += n;
	while (ref[*i])
    {
		if (!ft_strncmp(&ref[*i], "$((", 3))
			copy_till_end_of_nested(ref, buffer, i, j, 3);
		if (ref[*i] == '(')
			copy_till_end_of_nested(ref, buffer, i, j, 1);

		buffer[(*j)] = ref[(*i)];
		if (!ft_strncmp(&ref[*i], "))", 2)) {
			(*i) += 2; (*j) += 1;
			return;
		}
		if (ref[*i] == ')') {
			(*i) += 1; (*j) += 1;
			return;
        }
		(*i)++; (*j)++;
    }
}

static void replace_nested_greedy(const char *ref, char *buffer) {
	int i = 0;
	int j = 0;
	while (ref[i]) {
		if (!ft_strncmp(&ref[i], "$((", 3)) {
			copy_till_end_of_nested(ref, buffer, &i, &j, 3);
			continue;
		}
		buffer[j++] = ref[i++];
	}
	buffer[j] = '\0';
}

static Lexer * lexer_init(char *input) {
	Lexer * lexer = gc(GC_ADD, ft_calloc(1, sizeof(Lexer)), GC_SUBSHELL);

	Lexer self = {
		.input = input,
		.input_len = ft_strlen(input),
		.position = 0,
		.read_position = 0,
		.ch = 0,
	};

	*lexer = self;
	lexer_read_char(lexer);

	return lexer;
}

void aTokenListToString(ATokenStack *tokens);

char *arithmetic_expansion(char *const str, Vars *const shell_vars, int *error) {

	char *tmp = ft_strdup(str);
	replace_nested_greedy(tmp, str);
	FREE_POINTERS(tmp);

	long result = 0;
	Lexer * lexer = lexer_init(str);
	ATokenStack *token_stack = lexer_arithmetic_exp_lex_all(lexer);

	if (!token_stack) { 
		g_exitno = 1;
		return (*error) = 1, NULL; 
	}
	if (!token_stack->size) {
		goto empty;	
	}
	if (!arithmetic_syntax_check(token_stack)) {
		return (*error) = 1, NULL;
	}

	ATokenStack *token_queue = tokens_to_rpn(token_stack);
	ANode *AST = generate_atree(token_queue);
	result = aAST_execute(AST, shell_vars, error);
	if ((*error) == -1) {
		ft_dprintf(2, DIVISION_BY_0"\n"); g_exitno = 1;
		return (*error) = 1, NULL;
	}

	char *result_str = NULL;
empty: {
		result_str = ft_ltoa(result);
	}
	return result_str;
}
