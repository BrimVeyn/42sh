/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_execute.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:06:26 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/05 09:37:12 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

int ast_execute(Node *node, char **env) {
	if (node->tag == N_OPERATOR) {
		if (node->value.operator == S_AND) {
			int exit_status_lhs = ast_execute(node->left, env);
			if (exit_status_lhs == 0) {
				return ast_execute(node->right, env);
			} else {
				return exit_status_lhs;
			}
		}
		if (node->value.operator == S_OR) {
			int exit_status_lhs = ast_execute(node->left, env);
			if (exit_status_lhs != 0) {
				return ast_execute(node->right, env);
			} else {
				return exit_status_lhs;
			}
		}
	}
	if (node->tag == N_OPERAND) {
		return exec_node(node, env);
	}
	
	printf("oulalalalalalalla\n");
	exit(EXIT_FAILURE);
}
