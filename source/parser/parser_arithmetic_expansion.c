/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_arithmetic_expansion.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 15:46:07 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/27 16:52:59 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "arithmetic.h"
#include "ast.h"
#include "debug.h"
#include "lexer.h"
#include "libft.h"
#include "parser.h"
#include "utils.h"
#include "assert.h"
#include <stdio.h>

static void skip_parenthesis(char *str, int *i) {
	(*i)++;
	while (str[*i] && str[*i] != ')') {
		if (str[*i] == '(') {
			skip_parenthesis(str, i);
		}
		(*i)++;
	}
}

static int get_arithmetic_exp_range_end(char *str, int *i) {
	(*i) += 3; //skip '$(('

	while (str[*i] && ft_strncmp("))", &str[*i], 2)) {
		if (!ft_strncmp("$((", &str[*i], 3)) {
			get_arithmetic_exp_range_end(str, i);
			if (*i == -1) return -1;
		}
		if (str[*i] == '(') {
			skip_parenthesis(str, i);
		}
		(*i)++;
	}
	if (ft_strncmp(&str[*i], "))", 2)) return -1;
	(*i) += 1;
	return *i;
}

static Range get_arithmetic_exp_range(char *str) {
	Range self = {
		.start = -1,
		.end = -1,
	};

	self.start = ft_strstr(str, "$((");
	if (self.start != -1) {
		int start_copy = self.start;
		self.end = get_arithmetic_exp_range_end(str, &start_copy);
	}
	return self;
}

ATokenStack *tokens_to_rpn(ATokenStack *list) {
	ATokenStack *output = atoken_stack_init();
	ATokenStack *operator = atoken_stack_init();
	
	// aTokenListToString(list);
	for (int i = 0; i < list->size; i++) {
		AToken *current = list->data[i];
		if (is_lparen(current)) {
			atoken_stack_push(operator, current);
		} else if (is_rparen(current)) {
			while (true) {
				AToken *popped = atoken_stack_pop(operator);
				if (is_lparen(popped))
					break;
				atoken_stack_push(output, popped);
            }
		} else if (!is_aoperator(current)) {
			atoken_stack_push(output, current);
		} else {
			while (has_higher_prec(operator, current)) {
				atoken_stack_push(output, atoken_stack_pop(operator));
			}
			atoken_stack_push(operator, current);
		}
	}
	while (operator->size) {
		atoken_stack_push(output, atoken_stack_pop(operator));
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

bool parser_arithmetic_expansion(TokenList *tokens, Vars *shell_vars) {
	int i = 0;

	while (i < tokens->size) {
		Token *elem = tokens->t[i];

		//Goto next token if its not word
		if (!is_word(tokens, &i)) { i += 1; continue; }

		const Range range = get_arithmetic_exp_range(elem->w_infix);
		if (is_a_match(range)) {
			if (!is_range_valid(range, UNCLOSED_ARITMETIC_EXP_STR)) { g_exitno = 126; return false; }
			else { i += 1; continue; }
		}

		char infix[MAX_WORD_LEN] = {0};
		ft_memset(infix, '\0', MAX_WORD_LEN);
		ft_memcpy(infix, &elem->w_infix[range.start + 3], (range.end - range.start) - 4);
		char *tmp = ft_strdup(infix);
		replace_nested_greedy(tmp, infix);
		FREE_POINTERS(tmp);

		// printf(C_RED"------"C_RESET"\n");
		// printf("%s\n", infix);
		// printf(C_RED"------"C_RESET"\n");

		long result = 0;
		Lexer_p lexer = lexer_init(infix);
		ATokenStack *tokens = lexer_arithmetic_exp_lex_all(lexer);
		// aTokenListToString(tokens);

		if (!tokens->size) { goto empty; }
		if (!tokens) { g_exitno = 1; return false; }
		if (!arithmetic_syntax_check(tokens)) {
			exit(EXIT_FAILURE); // TODO: handle error properly
		}
		ATokenStack *token_queue = tokens_to_rpn(tokens);
		ANode *AST = generate_atree(token_queue);
		int error = 0;
		result = aAST_execute(AST, shell_vars, &error);
		if (error == -1) {
			dprintf(2, DIVISION_BY_0"\n"); g_exitno = 1;
			return false;
		}

	empty: {
			char *prefix = ft_substr(elem->w_infix, 0, range.start);
			char *postfix = ft_substr(elem->w_infix, range.end + 1, (strlen(elem->w_infix) - 1) - range.end);
			char *result_str = ft_itoa(result);
			char *prefix_result = ft_strjoin(prefix, result_str);
			char *prefix_result_postfix= ft_strjoin(prefix_result, postfix);
			FREE_POINTERS(prefix, postfix, result_str, prefix_result);
			elem->w_infix = gc_add(prefix_result_postfix, GC_SUBSHELL);
	}
	}
	return true;
}
