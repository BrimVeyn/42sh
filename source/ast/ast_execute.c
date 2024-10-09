/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_execute.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:11:53 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/09 09:18:07 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "exec.h"
#include <stdlib.h>
#include <stdio.h>

int ast_execute(Node *node, Vars *shell_vars, bool foreground) {
	if (node->tag == N_OPERATOR) {
		if (node->value.operator == S_AND) {
			int exit_lhs = ast_execute(node->left, shell_vars, true);
			if (exit_lhs == EXIT_SUCCESS) {
				int exit_rhs = ast_execute(node->right, shell_vars, true);
				return exit_rhs;
			} else
				return exit_lhs;
		} else if (node->value.operator == S_OR) {
			int exit_lhs = ast_execute(node->left, shell_vars, true);
			if (exit_lhs != EXIT_SUCCESS) {
				int exit_rhs = ast_execute(node->right, shell_vars, true);
				return exit_rhs;
			}
			else
				return exit_lhs;
		} else if (node->value.operator == S_SEMI_COLUMN) {
			int last_exit;
			if (node->left) {
				last_exit = ast_execute(node->left, shell_vars, true);
			}
			if (node->right) {
				last_exit = ast_execute(node->right, shell_vars, true);
			}
			return last_exit;
		} else if (node->value.operator == S_BG) {
			int last_exit;
			if (node->left) {
				last_exit = ast_execute(node->left, shell_vars, false);
			}
			if (node->right) {
				last_exit = ast_execute(node->right, shell_vars, true);
			}
			return last_exit;
		}
	}
	if (node->tag == N_OPERAND) {
		int exit_status = exec_node(node, shell_vars, foreground);
		return exit_status;
	}

	perror("AST FATAL");
	exit(EXIT_FAILURE);
}
