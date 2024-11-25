/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansions_debug.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 12:50:32 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/25 17:32:42 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "arithmetic.h"
#include "parser.h"

#include <stdio.h>

void exp_kind_list_print(ExpKindList *list) {
	static const char *map[] = {
		[EXP_WORD] = "EXP_WORD",
		[EXP_CMDSUB] = "EXP_CMDSUB",
		[EXP_ARITHMETIC] = "EXP_ARITHMETIC",
		[EXP_VARIABLE] = "EXP_VARIABLE",
		[EXP_SUB] = "EXP_SUB",
	};

	for (size_t i = 0; i < list->size; i++) {
		dprintf(2, "[%zu]: %s\n", i, map[list->data[i]]);
	}
}

void printCharChar(char **tab) {
	for (uint16_t it = 0; tab[it]; it++) {
		dprintf(2,"arg[%d]: %s\n", it, tab[it]);
	}
}


char *aOpToSTring(AToken *token) {
	static const struct {
		arithmetic_operators op;
		char *str;
	} map[] = {
		{O_PLUS, "+"}, {O_MINUS, "-"},
		{O_MULT, "*"}, {O_DIVIDE, "/"}, {O_MODULO, "%"},
		{O_POST_INCR, "x++"}, {O_POST_DECR, "x--"},
		{O_PREF_INCR, "++x"}, {O_PREF_DECR, "--x"},
		{O_LT, "<"}, {O_GT, ">"}, {O_LE, "<="}, {O_GE, ">="},
		{O_EQUAL, "=="}, {O_DIFFERENT, "!="},
		{O_AND, "&&"}, {O_OR, "||"}, 
		{O_POPEN, "("}, {O_PCLOSE, ")"},
	};

	for (size_t i = 0; i < sizeof(map) / sizeof(map[0]); i++) {
		if (token->operator == map[i].op) return map[i].str;
	}
	return "invalid";
}

void aTokenToString(AToken *token) {
	switch (token->tag) {
		case A_OPERAND: 
			switch (token->operand_tag) {
				case VALUE:
					dprintf(2, "%ld ", token->litteral);
					break;
				case VARIABLE:
					dprintf(2, "%s ", token->variable);
					break;
			}
			break;
		case A_OPERATOR:
			dprintf(2, "%s ", aOpToSTring(token));
			break;
		default:
			dprintf(2, "UNHANDLED FORMAT !\n");
			break;
	}
}

void aTokenListToString(ATokenStack *tokens) {
	dprintf(2, "-------------Print start--------\n");
	for (int i = 0; i < tokens->size; i++) {
		aTokenToString(tokens->data[i]);
	}
	dprintf(2, "\n");
	dprintf(2, "-------------Print end--------\n");
}
