/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_arithmetic_expansion.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 15:46:07 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/25 17:36:10 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "arithmetic.h"
#include "ast.h"
#include "debug.h"
#include "lexer.h"
#include "utils.h"
#include "assert.h"
#include <stdio.h>

int get_arithmetic_exp_range_end(char *str, int *i, Range *range) {
	(*i) += 3; //skip '$(('

	while (str[*i] && ft_strncmp("))", &str[*i], 2)) {
		if (!ft_strncmp("$((", &str[*i], 3)) {
			range->start = *i;
			return get_arithmetic_exp_range_end(str, i, range);
			if (*i == -1) return -1;
		}
		(*i)++;
	}
	if (ft_strncmp(&str[*i], "))", 2)) return -1;
	(*i) += 1;
	return *i;
}

Range get_arithmetic_exp_range(char *str) {
	Range self = {
		.start = -1,
		.end = -1,
	};

	self.start = ft_strstr(str, "$((");
	if (self.start != -1) {
		int start_copy = self.start;
		self.end = get_arithmetic_exp_range_end(str, &start_copy, &self);
	}
	return self;
}

//Shunting yard algorithm
typedef enum {
	P0 = 6, // - 'a++' 'a--'
	P1 = 5, // - '++a' '--a'
	P3 = 4, // - '*' '/' '%'
	P2 = 3, // - '+' '-'
	P4 = 2, // - '<=' '>=' '<' '>'
	P5 = 1, // - '==' '!='
	P6 = 0, // - '&&' '||'
} operator_precedence;

operator_precedence get_precedence(AToken *token) {
	static const struct {
		arithmetic_operators OP;
		operator_precedence P;
	} pre[] = {
		{O_POST_DECR, P0}, {O_POST_INCR, P0},
		{O_PREF_DECR, P1}, {O_PREF_INCR, P1},
		{O_PLUS, P2}, {O_MINUS, P2},
		{O_MULT, P3}, {O_DIVIDE, P3}, {O_MODULO, P3},
		{O_LE, P4}, {O_GE, P4}, {O_LT, P4}, {O_GT, P4},
		{O_EQUAL, P5}, {O_DIFFERENT, P5},
		{O_AND, P6}, {O_OR, P6},
	};

	for (size_t i = 0; i < sizeof(pre) / sizeof(pre[0]); i++) {
		if (token->operator == pre[i].OP) return pre[i].P;
	}
	return P0;
}

bool is_aoperator(const AToken *token) {
	return (token->tag == A_OPERATOR);
}

bool has_higher_prec(ATokenStack *operator, AToken *current) {
	if (!operator->size) return false;
	return (get_precedence(operator->data[operator->size - 1]) >= get_precedence(current));
}

ATokenStack *tokens_to_rpn(ATokenStack *list) {
	ATokenStack *output = atoken_stack_init();
	ATokenStack *operator = atoken_stack_init();
	
	for (int i = 0; i < list->size; i++) {
		AToken *current = list->data[i];
		if (!is_aoperator(current)) {
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

AToken *gen_value_token(long value) {
	AToken *self = gc_add(ft_calloc(1, sizeof(Token *)), GC_SUBSHELL);
	self->tag = A_OPERAND;
	self->operand_tag = VALUE;
	self->litteral = value;
	return self;
}

ANode *gen_aoperator_node(AToken *tok, ANode *left, ANode *right) {
	ANode *self = gc_add(ft_calloc(1, sizeof(ANode)), GC_SUBSHELL);
	self->value = tok;
	self->left = left;
	self->right = right;
	return self;
}

ANode *gen_aoperand_node(AToken *tok) {
	ANode *self = gc_add(ft_calloc(1, sizeof(ANode)), GC_SUBSHELL);
	self->value = tok;
	self->left = NULL;
	self->right = NULL;
	return self;
}

ANode *generate_atree(ATokenStack *list) {
	ANodeStack *self = anode_stack_init();
	for (int i = 0; i < list->size; i++) {
		if (!is_aoperator(list->data[i])) {
			anode_stack_push(self, gen_aoperand_node(list->data[i]));
		} else {
			if (self->size == 1) {
				ANode *left = anode_stack_pop(self);
				anode_stack_push(self, gen_aoperator_node(list->data[i], left, NULL));
			} else {
				ANode *right = anode_stack_pop(self);
				ANode *left = anode_stack_pop(self);
				anode_stack_push(self, gen_aoperator_node(list->data[i], left, right));
			}
		}
	}
	assert(self->size == 1);
	return anode_stack_pop(self);
}

ANode *incr(ANode *self, Vars *shell_vars) {
	if (self->value->operand_tag == VARIABLE) {
		char tmp[MAX_WORD_LEN];
		self->value->litteral += 1;
		ft_sprintf(tmp, "%s=%d", self->value->variable, self->value->litteral);
		dprintf(2, "str: %s\n", tmp);
		string_list_add_or_update(shell_vars->set, strdup(tmp));
	}
	return self;
}

long aAST_execute(ANode *node, Vars *shell_vars) {
	if (!node) return 0;

	switch (node->value->tag) {
		case A_OPERATOR:
			switch (node->value->operator) {
				case O_PREF_INCR: {
					return aAST_execute(incr(node->left, shell_vars), shell_vars);
				}
				case O_PLUS:
					return aAST_execute(node->left, shell_vars) + aAST_execute(node->right, shell_vars);
				default:
					exit(EXIT_FAILURE);
		}
		case A_OPERAND:
			return node->value->litteral; //GET_VALUE;
		default:
			exit(EXIT_FAILURE);
	}
}



bool is_incr_or_decr(const AToken *token) {
	return (is_aoperator(token) && (token->operator == O_PREF_INCR || token->operator == O_PREF_DECR));
}

bool arithmetic_syntax_check(ATokenStack *list) {
	for (int i = 0; i < list->size; i++) {
		const AToken *prev = i > 0 ? list->data[i - 1] : NULL;
		AToken *current = list->data[i];
		const AToken *next = i < list->size - 1 ? list->data[i + 1] : NULL;

		if (is_incr_or_decr(current)) {
			if (prev && !is_aoperator(prev)) {
				current->operator = (current->operator == O_PREF_DECR) ? O_POST_DECR : O_POST_INCR;
			} else if (next && is_aoperator(next)) {
				dprintf(2, "syntax error !\n");
				return false;
			}
		}
	}
	return true;
}

bool parser_arithmetic_expansion(TokenList *tokens, Vars *shell_vars) {
	int i = 0;

	while (i < tokens->size) {
		Token *elem = tokens->t[i];

		if (!is_word(tokens, &i)) {
			i += 1;
			continue;
		}

		const Range range = get_arithmetic_exp_range(elem->w_infix);
		// dprintf(2, "r.start: %d, r.end: %d\n", range.start, range.end);
		if (is_a_match(range)) {
			if (!is_range_valid(range, UNCLOSED_ARITMETIC_EXP_STR)) {
				g_exitno = 126;
				return false;
			} else {
				i += 1; 
				continue;
			}
		}

		char infix[MAX_WORD_LEN] = {0};
		ft_memcpy(infix, &elem->w_infix[range.start + 3], (range.end - range.start) - 4);
		// printf("%s\n", infix);

		Lexer_p lexer = lexer_init(infix);
		ATokenStack *tokens = lexer_arithmetic_exp_lex_all(lexer, shell_vars);
		if (!arithmetic_syntax_check(tokens)) {
			exit(EXIT_FAILURE); // TODO handle error properly
		}
		// aTokenListToString(tokens);
		if (!tokens) {
			i += 1;
			continue;
		}
		// (void) shell_vars;
		ATokenStack *token_queue = tokens_to_rpn(tokens);
		aTokenListToString(token_queue);
		ANode *AST = generate_atree(token_queue);
		long result = aAST_execute(AST, shell_vars);
		//Freee old w_infix, handle prefix and suffix
		elem->w_infix = gc_add(ft_itoa(result), GC_SUBSHELL);
		// dprintf(2, "result = %ld\n", result);
		// dprintf(2, "infix = %s\n", infix);

		i++;
	}
	return true;
}
