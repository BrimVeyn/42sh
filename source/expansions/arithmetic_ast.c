/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arithmetic_ast.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 11:11:06 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/09 12:24:36 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "arithmetic.h"
#include "final_parser.h"
#include "utils.h"
#include "libft.h"
#include <unistd.h>


static long get_value(const ANode * const self, const Vars * const shell_vars) {
	if (self->value->operand_tag == VARIABLE) {
		const char * const str_value = string_list_get_value(shell_vars->set, self->value->variable);
		const long int_value = (str_value == NULL) ? 0 : ft_atol(str_value);
		return int_value;
	} else {
		return self->value->litteral;
	}
}

static ANode *incr(ANode * const self, const Vars * const shell_vars) {
	if (self->value->operand_tag == VARIABLE) {
		char tmp[MAX_WORD_LEN] = {0};
		const long var_value = get_value(self, shell_vars);
		if (ft_snprintf(tmp, MAX_WORD_LEN, "%s=%ld", self->value->variable, var_value + 1) == -1)
			fatal("snprintf: buffer overflow", __LINE__, __FILE_NAME__, 1);
		const char * const maybe_value = string_list_get_value(shell_vars->env, self->value->variable);
		if (maybe_value)
			string_list_add_or_update(shell_vars->env, tmp);
		string_list_add_or_update(shell_vars->set, tmp);
	}
	return self;
}

static ANode *decr(ANode * const self, const Vars * const shell_vars) {
	if (self->value->operand_tag == VARIABLE) {
		char tmp[MAX_WORD_LEN] = {0};
		const long var_value = get_value(self, shell_vars);
		if (ft_snprintf(tmp, MAX_WORD_LEN, "%s=%ld", self->value->variable, var_value - 1) == -1)
			fatal("snprintf: buffer overflow", __LINE__, __FILE_NAME__, 1);
		const char * const maybe_value = string_list_get_value(shell_vars->env, self->value->variable);
		if (maybe_value)
			string_list_add_or_update(shell_vars->env, tmp);
		string_list_add_or_update(shell_vars->set, tmp);
	}
	return self;
}

static ANode *gen_aoperator_node(AToken * const tok, ANode * const left, ANode * const right) {
	ANode * const self = gc(GC_ADD, ft_calloc(1, sizeof(ANode)), GC_SUBSHELL);
	self->value = tok;
	self->left = left;
	self->right = right;
	return self;
}

static ANode *gen_aoperand_node(AToken * const tok) {
	ANode * const self = gc(GC_ADD, ft_calloc(1, sizeof(ANode)), GC_SUBSHELL);
	self->value = tok;
	self->left = NULL;
	self->right = NULL;
	return self;
}

ANode *generate_atree(const ATokenStack * const list) {
	da_create(self, ANodeStack, sizeof(ANode *), GC_SUBSHELL);
	for (int i = 0; i < list->size; i++) {
		if (!is_aoperator(list->data[i])) {
			da_push(self, gen_aoperand_node(list->data[i]));
		} else {
			if (self->size == 1) {
				ANode *left = da_pop(self);
				da_push(self, gen_aoperator_node(list->data[i], left, NULL));
			} else {
				ANode *right = da_pop(self);
				ANode *left = da_pop(self);
				da_push(self, gen_aoperator_node(list->data[i], left, right));
			}
		}
	}
	return da_pop(self);
}

long aAST_execute(const ANode * const node, Vars * const shell_vars, int * const error) {
	if (!node) return 0;

	switch (node->value->tag) {
		case A_OPERATOR: {
			switch (node->value->operator) {
				case O_PREF_INCR: return aAST_execute(incr(node->left, shell_vars), shell_vars, error);
				case O_POST_INCR: {
					const long var_value = get_value(node->left, shell_vars);
					incr(node->left, shell_vars);
					return var_value;
				}
				case O_PREF_DECR: return aAST_execute(decr(node->left, shell_vars), shell_vars, error);
				case O_POST_DECR: {
					const long var_value = get_value(node->left, shell_vars);
					decr(node->left, shell_vars);
					return var_value;
				}
				case O_PLUS: return aAST_execute(node->left, shell_vars, error) + aAST_execute(node->right, shell_vars, error);
				case O_MINUS: return aAST_execute(node->left, shell_vars, error) - aAST_execute(node->right, shell_vars, error);
				case O_MULT: return aAST_execute(node->left, shell_vars, error) * aAST_execute(node->right, shell_vars, error);
				case O_DIVIDE: {
					const long left_result = aAST_execute(node->left, shell_vars, error);
					const long right_result = aAST_execute(node->right, shell_vars, error);
					if (right_result == 0) {
						*error = -1;
						return 0;
					}
					return left_result / right_result;
                }
				case O_MODULO: {
					const long left_result = aAST_execute(node->left, shell_vars, error);
					const long right_result = aAST_execute(node->right, shell_vars, error);
					if (right_result == 0) {
						*error = -1;
						return 0;
					}
					return left_result % right_result;
                }
				case O_GE: return aAST_execute(node->left, shell_vars, error) >= aAST_execute(node->right, shell_vars, error);
				case O_LE: return aAST_execute(node->left, shell_vars, error) <= aAST_execute(node->right, shell_vars, error);
				case O_GT: return aAST_execute(node->left, shell_vars, error) > aAST_execute(node->right, shell_vars, error);
				case O_LT: return aAST_execute(node->left, shell_vars, error) < aAST_execute(node->right, shell_vars, error);
				case O_EQUAL: return aAST_execute(node->left, shell_vars, error) == aAST_execute(node->right, shell_vars, error);
				case O_DIFFERENT: return aAST_execute(node->left, shell_vars, error) != aAST_execute(node->right, shell_vars, error);
				case O_AND: return aAST_execute(node->left, shell_vars, error) && aAST_execute(node->right, shell_vars, error);
				case O_OR: return aAST_execute(node->left, shell_vars, error) || aAST_execute(node->right, shell_vars, error);
				default: fatal("arithmetic AST: unhandled case", __LINE__, __FILE_NAME__, 1);
			}
			return 0;
		}
		case A_OPERAND: {
			return get_value(node, shell_vars);
		}
		default:
			exit(EXIT_FAILURE);
	}
}
