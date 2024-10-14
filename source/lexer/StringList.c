/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringList.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 14:12:30 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/14 17:00:08 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "utils.h"
#include "libft.h"

#include <stdio.h>

void add_vars_to_set(Vars *shell_vars, TokenList *vars) {
	StringList *set = shell_vars->set;
	StringList *env = shell_vars->env;
	for (size_t i = 0; i < vars->size; i++) {
		string_list_update(env, vars->data[i]->w_infix);
		string_list_add_or_update(set, vars->data[i]->w_infix);
	}
}

void add_vars_to_local(StringList *list, TokenList *vars) {
	for (size_t i = 0; i < vars->size; i++) {
		string_list_add_or_update(list, vars->data[i]->w_infix);
	}
}

void string_list_clear(StringList *list) {
	for (size_t i = 0; i < list->size; i++) {
		gc(GC_FREE, list->data[i], GC_ENV);
		list->data[i] = NULL;
	}
	list->size = 0;
}

void string_list_delete(StringList *list) {
	string_list_clear(list);
	gc(GC_FREE, list->data, GC_ENV);
	gc(GC_FREE, list, GC_ENV);
}

bool string_list_remove(StringList *sl, char *id) {
	for (size_t i = 0; i < sl->size; i++) {
		char *curr_equal_ptr = ft_strchr(sl->data[i], '=');
		size_t curr_equal_pos = curr_equal_ptr - sl->data[i];
		char *curr_id = ft_substr(sl->data[i], 0, curr_equal_pos);

		if  (!ft_strcmp(curr_id, id)) {
			gc(GC_FREE, sl->data[i], GC_ENV);
			sl->data[i] = sl->data[sl->size - 1];
			sl->data[sl->size - 1] = NULL;
			sl->size--;
			return true;
		}
		free(curr_id);
	}
	return false;
}

bool string_list_update(StringList *sl, char *var) {
	char *equal_ptr = ft_strchr(var, '=');
	bool add_only = (!equal_ptr);
	size_t equal_pos = equal_ptr - var;
	char *id = ft_substr(var, 0, equal_pos);

	for (size_t i = 0; i < sl->size; i++) {
		char *curr_equal_ptr = ft_strchr(sl->data[i], '=');
		size_t curr_equal_pos = curr_equal_ptr - sl->data[i];
		char *curr_id = ft_substr(sl->data[i], 0, curr_equal_pos);

		if  (!ft_strcmp(curr_id, id)) {
			if (!add_only) {
				gc(GC_FREE, sl->data[i], GC_ENV);
				sl->data[i] = gc(GC_ADD, ft_strdup(var), GC_ENV);
            }
			FREE_POINTERS(curr_id, id);
			return true;
		}
		free(curr_id);
	}
	free(id);
	return false;
}

void string_list_add_or_update(StringList *sl, char *var) {
	if (!string_list_update(sl, var))
		da_push(sl, var);
}

char *string_list_get_value(StringList *sl, char *id) {
	for (size_t i = 0; i < sl->size; i++) {
		char *curr_equal_ptr = ft_strchr(sl->data[i], '=');
		size_t curr_equal_pos = curr_equal_ptr - sl->data[i];
		char *curr_id = ft_substr(sl->data[i], 0, curr_equal_pos);

		if  (!ft_strcmp(curr_id, id)) {
			free(curr_id);
			return &sl->data[i][curr_equal_pos + 1];
		}
		free(curr_id);
	}
	return NULL;
}

char *shell_vars_get_value(Vars *shell_vars, char *id) {
	char *return_value = string_list_get_value(shell_vars->env, id);
	if (!return_value) {
		return_value = string_list_get_value(shell_vars->set, id);
	}
	return return_value;
}

void string_list_print(const StringList *list) {
	for (size_t i = 0; i < list->size; i++) {
		char *eql = ft_strchr(list->data[i], '=');
		if (!eql) continue;
		printf("%s\n", list->data[i]);
	}
}
