/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_parser.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 12:50:32 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/04 12:50:41 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

void printRedirList(RedirectionList *rl) {
	printf(C_BRONZE"------ "C_LIGHT_BROWN"Redir list"C_BRONZE"----\n"C_RESET);
	for (uint16_t it = 0; it < rl->size; it++) {
		const Redirection *el = rl->r[it];
		printf("prefix_fd ["C_BRONZE"%d"C_RESET"]:\t"C_LIGHT_BROWN"%d\n"C_RESET, it, el->prefix_fd);
		printf("r_type    ["C_BRONZE"%d"C_RESET"]:\t"C_LIGHT_BROWN"%s\n"C_RESET, it, tagName(el->r_type));
		printf("su_type   ["C_BRONZE"%d"C_RESET"]:\t"C_LIGHT_BROWN"%s\n"C_RESET, it, tagName(el->su_type));
		if (el->su_type == R_FD) {
			printf("suffix_fd ["C_BRONZE"%d"C_RESET"]:"C_LIGHT_BROWN"\t%d\n"C_RESET, it, el->fd);
		} else if (el->su_type == R_FILENAME) {
			printf("filename  ["C_BRONZE"%d"C_RESET"]:\t"C_LIGHT_BROWN"%s\n"C_RESET, it, el->filename);
		}
		if (it + 1 < rl->size) {
			printf(C_BRONZE"------------------\n"C_RESET);
		}
	}
	printf(C_BRONZE"---------...---------\n"C_RESET);

}

void printCharChar(char **tab) {
	for (uint16_t it = 0; tab[it]; it++) {
		printf("arg[%d]: %s\n", it, tab[it]);
	}
}

void printCommand(SimpleCommand *command) {
	SimpleCommand *curr = command;
	while (curr != NULL) {
		printRedirList(curr->redir_list);
		printf(C_GOLD"------ "C_LIGHT_YELLOW"Command"C_RESET C_GOLD"-------\n"C_RESET);
		printf("bin: %s\n", command->bin);
		printCharChar(curr->args);
		printf(C_GOLD"---------...---------"C_RESET"\n");
		curr = curr->next;
	}
}
