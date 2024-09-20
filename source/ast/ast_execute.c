/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_execute.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:11:53 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/20 13:55:56 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "exec.h"

#include <stdlib.h>
#include <stdio.h>

int ast_execute(Node *node, Vars *shell_vars) {
	if (node->tag == N_OPERATOR) {
		if (node->value.operator == S_AND) {
			int exit_lhs = ast_execute(node->left, shell_vars);
			if (exit_lhs == EXIT_SUCCESS) {
				int exit_rhs = ast_execute(node->right, shell_vars);
				// printf(C_GREEN);
				// printTree(node->left);
				// printTree(node->right);
				// printf("Right Side AND = %d\n", exit_rhs);
				// printf("PID = %d\n", getpid());
				// printf(C_RESET"\n");
				return exit_rhs;
			} else
				return exit_lhs;
		}
		if (node->value.operator == S_OR) {
			int exit_lhs = ast_execute(node->left, shell_vars);
			// printf(C_GREEN);
			// printTree(node->left);
			// printTree(node->right);
			// printf("Left Side OR = %d\n", exit_lhs);
			// printf("PID = %d\n", getpid());
			// printf(C_RESET"\n");
			if (exit_lhs != EXIT_SUCCESS) {
				int exit_rhs = ast_execute(node->right, shell_vars);
				return exit_rhs;
			}
			else
				return exit_lhs;
		}
		if (node->value.operator == S_SEMI_COLUMN || node->value.operator == S_BG) {
			int last_exit;
			if (node->left) {
				last_exit = ast_execute(node->left, shell_vars);
			}
			if (node->right) {
				last_exit = ast_execute(node->right, shell_vars);
			}
			return last_exit;
		}
	}
	if (node->tag == N_OPERAND) {
		// printf("executing ---------------------------\n");
		// printTree(node);
		int exit_status = exec_node(node, shell_vars);
		// printf("exit status = %d -----------------------------\n", exit_status);
		return exit_status;
	}
	
	printf("FATAL AST ERROR\n");
	exit(EXIT_FAILURE);
}
