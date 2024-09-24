/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 12:50:32 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/24 15:55:12 by bvan-pae         ###   ########.fr       */
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

void aTokenToString(AToken *token) {
	switch (token->tag) {
		case A_OPERAND:
			dprintf(2, "OPERAND\n");
			break;
		case A_OPERATOR:
			dprintf(2, "OPERATOR\n");
			break;
		default:
			dprintf(2, "UNHANDLED FORMAT !\n");
			break;
	}
}

void aTokenListToString(ATokenStack *tokens) {
	for (int i = 0; i < tokens->size; i++) {
		aTokenToString(tokens->data[i]);
	}
}
