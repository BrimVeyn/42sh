/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_execute.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:06:26 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/03 16:53:23 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

int ast_execute(Node *node) {
	if (node->tag == N_OPERATOR) {
		if (node->value.operator == S_AND) {
			int exit_status_lhs = ast_execute(node->left);
			if (exit_status_lhs == 0) {
				return ast_execute(node->right);
			} else {
				return exit_status_lhs;
			}
		}
		if (node->value.operator == S_OR) {
			int exit_status_lhs = ast_execute(node->left);
			if (exit_status_lhs != 0) {
				return ast_execute(node->right);
			} else {
				return exit_status_lhs;
			}
		}
	}
	if (node->tag == N_OPERAND) {
		return exec_node(node, __environ);
	}
	
	printf("oulalalalalalalla\n");
	exit(EXIT_FAILURE);
}
