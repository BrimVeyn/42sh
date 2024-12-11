/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arithmetic_syntax.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 11:18:31 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/11 10:46:29 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "arithmetic.h"
#include "parser.h"

#include <stdio.h>
#include <unistd.h>



static bool is_incr_or_decr(const AToken *token) {
	return (is_aoperator(token) && (token->operator <= O_PREF_DECR && token->operator >= O_POST_INCR));
}

bool is_aoperator(const AToken *token) {
	return (token->tag == A_OPERATOR);
}

bool is_abinary_op(const AToken *token) {
	return (is_aoperator(token) && (token->operator > O_PREF_DECR));
}

bool is_lparen(const AToken *token) {
	return (token->tag == A_OPERATOR && token->operator == O_POPEN);
}

bool is_rparen(const AToken *token) {
	return (token->tag == A_OPERATOR && token->operator == O_PCLOSE);
}

bool arithmetic_syntax_check(ATokenStack *list) {
	int parenthesis = 0; //+1 if ( -1 if )

	for (int i = 0; i < list->size; i++) {
		const AToken *prev = i > 0 ? list->data[i - 1] : NULL;
		AToken *current = list->data[i];
		const AToken *next = i < list->size - 1 ? list->data[i + 1] : NULL;
		const AToken *nextnext = i < list->size - 2 ? list->data[i + 2] : NULL;

		if (is_lparen(current)) parenthesis++;
		if (is_rparen(current)) parenthesis--;

		if (next && !is_aoperator(current) && !is_aoperator(next)) {
			ARITHMETIC_SYNTAX_ERROR("a number");
			return false;
		}
		if (prev && is_lparen(current) && (!is_lparen(prev) && !is_aoperator(prev))) {
			ARITHMETIC_SYNTAX_ERROR("(");
			return false;
		}
		if (next && is_rparen(current) && (!is_rparen(next) && !is_aoperator(next))) {
			ARITHMETIC_SYNTAX_ERROR(")");
			return false;
		}
		if (is_incr_or_decr(current)) {
			if (!((prev && !is_aoperator(prev)) || (next && !is_aoperator(next)))) {
				ft_dprintf(2, OPERAND_EXPECTED"\n");
				return false;
			}
		}
		if (is_lparen(current) && is_rparen(next)) {
			ft_dprintf(2, OPERAND_EXPECTED"\n");
			return false;
		}
		if (is_abinary_op(current)) {
			if (!next || !prev || (is_aoperator(prev) && is_abinary_op(prev)) || (is_aoperator(next) && is_abinary_op(next))) {
				ft_dprintf(2, OPERAND_EXPECTED"\n");
				return false;
			}  
		}
		if (nextnext && is_incr_or_decr(current) && !is_aoperator(next) && is_incr_or_decr(nextnext)) {
			ft_dprintf(2, ASSIGNMENT_REQUIRES_LVALUE"++\")\n");
			return false;
		}
		if (is_incr_or_decr(current)) {
			if (prev && !is_aoperator(prev)) {
				current->operator = (current->operator == O_PREF_DECR) ? O_POST_DECR : O_POST_INCR;
			} else if (next && is_aoperator(next)) {
				ft_dprintf(2, OPERAND_EXPECTED"\n");
				return false;
			}
		}
	}
	if (parenthesis != 0) {
		if (parenthesis > 0)
			ARITHMETIC_UNCLOSED_PAREN(")");
		if (parenthesis < 0)
			ARITHMETIC_UNCLOSED_PAREN("(");
		return false;
	}
	return true;
}
