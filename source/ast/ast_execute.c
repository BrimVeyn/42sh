/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_execute.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:06:26 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/05 14:45:10 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

int ast_execute(Node *node, char **env) {
	if (node->tag == N_OPERATOR) {
		if (node->value.operator == S_AND) {
			int exit_lhs = ast_execute(node->left, env);
			if (exit_lhs == EXIT_SUCCESS)
				return ast_execute(node->right, env);
			else
				return exit_lhs;
		}
		if (node->value.operator == S_OR) {
			int exit_lhs = ast_execute(node->left, env);
			if (exit_lhs != EXIT_SUCCESS)
				return ast_execute(node->right, env);
			else
				 return exit_lhs;
		}
		if (node->value.operator == S_SEMI_COLUMN || node->value.operator == S_BG) {
			int last_exit;
			if (node->left) {
				last_exit = ast_execute(node->left, env);
			}
			if (node->right) {
				last_exit = ast_execute(node->right, env);
			}
			return last_exit;
		}
	}
	if (node->tag == N_OPERAND) {
		return exec_node(node, env);
	}
	
	printf("oulalalalalalalla\n");
	exit(EXIT_FAILURE);
}
