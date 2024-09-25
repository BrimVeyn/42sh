/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 12:50:32 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/25 16:56:03 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/debug.h"
#include "../../include/colors.h"

#include <stdio.h>

void printRedirList(RedirectionList *rl) {
	dprintf(2,C_BRONZE"------ "C_LIGHT_BROWN"Redir list"C_BRONZE"----\n"C_RESET);
	for (uint16_t it = 0; it < rl->size; it++) {
		const Redirection *el = rl->r[it];
		dprintf(2,"prefix_fd ["C_BRONZE"%d"C_RESET"]:\t"C_LIGHT_BROWN"%d\n"C_RESET, it, el->prefix_fd);
		dprintf(2,"r_type    ["C_BRONZE"%d"C_RESET"]:\t"C_LIGHT_BROWN"%s\n"C_RESET, it, tagName(el->r_type));
		dprintf(2,"su_type   ["C_BRONZE"%d"C_RESET"]:\t"C_LIGHT_BROWN"%s\n"C_RESET, it, tagName(el->su_type));
		if (el->su_type == R_FD) {
			dprintf(2,"suffix_fd ["C_BRONZE"%d"C_RESET"]:"C_LIGHT_BROWN"\t%d\n"C_RESET, it, el->fd);
		} else if (el->su_type == R_FILENAME) {
			dprintf(2,"filename  ["C_BRONZE"%d"C_RESET"]:\t"C_LIGHT_BROWN"%s\n"C_RESET, it, el->filename);
		}
		if (it + 1 < rl->size) {
			dprintf(2,C_BRONZE"------------------\n"C_RESET);
		}
	}
	dprintf(2,C_BRONZE"---------...---------\n"C_RESET);

}

void printCharChar(char **tab) {
	for (uint16_t it = 0; tab[it]; it++) {
		dprintf(2,"arg[%d]: %s\n", it, tab[it]);
	}
}

void printCommand(SimpleCommand *command) {
	SimpleCommand *curr = command;
	printRedirList(curr->redir_list);
	dprintf(2,C_GOLD"------ "C_LIGHT_YELLOW"Command"C_RESET C_GOLD"-------\n"C_RESET);
	dprintf(2,"bin: %s\n", command->bin);
	printCharChar(curr->args);
	dprintf(2,C_GOLD"---------...---------"C_RESET"\n");
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
	};

	for (size_t i = 0; i < sizeof(map) / sizeof(map[0]); i++) {
		if (token->operator == map[i].op) return map[i].str;
	}
	return "invalid";
}

void aTokenToString(AToken *token) {
	switch (token->tag) {
		case A_OPERAND:
			dprintf(2, "%ld ", token->litteral);
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
