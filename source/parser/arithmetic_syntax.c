/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arithmetic_syntax.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 11:18:31 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/30 11:38:56 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "arithmetic.h"
#include <stdio.h>

operator_precedence get_precedence(const AToken *token) {
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
		{O_PCLOSE, PP}, {O_POPEN, PP},
	};

	for (size_t i = 0; i < sizeof(pre) / sizeof(pre[0]); i++) {
		if (token->operator == pre[i].OP) return pre[i].P;
	}
	return P0;
}

bool is_aoperator(const AToken *token) {
	return (token->tag == A_OPERATOR);
}

bool is_abinary_op(const AToken *token) {
	return (is_aoperator(token) && (token->operator > O_PREF_DECR));
}

bool is_incr_or_decr(const AToken *token) {
	return (is_aoperator(token) && (token->operator <= O_PREF_DECR && token->operator >= O_POST_INCR));
}

bool is_lparen(const AToken *token) {
	return (token->tag == A_OPERATOR && token->operator == O_POPEN);
}

bool is_rparen(const AToken *token) {
	return (token->tag == A_OPERATOR && token->operator == O_PCLOSE);
}

bool has_higher_prec(const ATokenStack *operator, const AToken *current) {
	if (!operator->size) return false;
	return (get_precedence(operator->data[operator->size - 1]) >= get_precedence(current));
}

bool arithmetic_syntax_check(ATokenStack *list) {
	for (int i = 0; i < list->size; i++) {
		const AToken *prev = i > 0 ? list->data[i - 1] : NULL;
		AToken *current = list->data[i];
		const AToken *next = i < list->size - 1 ? list->data[i + 1] : NULL;
		const AToken *nextnext = i < list->size - 2 ? list->data[i + 2] : NULL;


		if (is_incr_or_decr(current)) {
			if (!((prev && !is_aoperator(prev)) || (next && !is_aoperator(next)))) {
				dprintf(2, OPERAND_EXPECTED"\n");
				return false;
			}
		}
		if (is_lparen(current) && is_rparen(next)) {
			dprintf(2, OPERAND_EXPECTED"\n");
			return false;
		}
		if (is_abinary_op(current)) {
			if (!next || !prev || (is_aoperator(prev) && is_abinary_op(prev)) || (is_aoperator(next) && is_abinary_op(next))) {
				dprintf(2, OPERAND_EXPECTED"\n");
				return false;
			}  
		}
		if (nextnext && is_incr_or_decr(current) && !is_aoperator(next) && is_incr_or_decr(nextnext)) {
			dprintf(2, ASSIGNMENT_REQUIRES_LVALUE"++\")\n");
			return false;
		}
		if (is_incr_or_decr(current)) {
			if (prev && !is_aoperator(prev)) {
				current->operator = (current->operator == O_PREF_DECR) ? O_POST_DECR : O_POST_INCR;
			} else if (next && is_aoperator(next)) {
				dprintf(2, OPERAND_EXPECTED"\n");
				return false;
			}
		}
	}
	return true;
}
