/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringList.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 14:12:30 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/02 14:15:44 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "lexer.h"
#include "utils.h"
#include "libft.h"

#include <stdio.h>

char *get_variable_value(Vars * const shell_vars, char * const name){
	char *result = string_list_get_value(shell_vars->positional, name);
	if (result) return result;
	result = string_list_get_value(shell_vars->local, name);
	if (result) return result;
	result = string_list_get_value(shell_vars->env, name);
	if (result) return result;
	result = string_list_get_value(shell_vars->set, name);
	return result;
}

void add_vars_to_set(const Vars * const shell_vars, const StringListL * const vars) {
	StringListL * const set = shell_vars->set;
	StringListL * const env = shell_vars->env;
	for (size_t i = 0; i < vars->size; i++) {
		string_list_update(env, vars->data[i]);
		string_list_add_or_update(set, vars->data[i]);
	}
}

void add_vars_to_local(StringListL * const list, const StringListL * const vars) {
	for (size_t i = 0; i < vars->size; i++) {
		string_list_add_or_update(list, vars->data[i]);
	}
}

void string_list_clear(StringListL * const list) {
	for (size_t i = 0; i < list->size; i++) {
		gc(GC_FREE, list->data[i], GC_ENV);
		list->data[i] = NULL;
	}
	list->size = 0;
}

void string_list_delete(StringListL * const list) {
	string_list_clear(list);
	gc(GC_FREE, list->data, GC_ENV);
	gc(GC_FREE, list, GC_ENV);
}

bool string_list_remove(StringListL * const sl, char * const id) {
	for (size_t i = 0; i < sl->size; i++) {
		const char * const curr_equal_ptr = ft_strchr(sl->data[i], '=');
		const size_t curr_equal_pos = curr_equal_ptr - sl->data[i];
		char * const curr_id = ft_substr(sl->data[i], 0, curr_equal_pos);

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

bool string_list_update(StringListL *sl, const char *var) {
	const char * const equal_ptr = ft_strchr(var, '=');
	const bool add_only = (!equal_ptr);
	const size_t equal_pos = equal_ptr - var;
	char * const id = ft_substr(var, 0, equal_pos);

	for (size_t i = 0; i < sl->size; i++) {
		const char *curr_equal_ptr = ft_strchr(sl->data[i], '=');
		const  size_t curr_equal_pos = curr_equal_ptr - sl->data[i];
		char * const curr_id = ft_substr(sl->data[i], 0, curr_equal_pos);

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

void string_list_add_or_update(StringListL * const sl, char * const var) {
	if (!string_list_update(sl, var))
		da_push(sl, gc(GC_ADD, ft_strdup(var), GC_ENV));
}

void string_list_append(const StringListL * const sl, char * const var) {
	const size_t plus_pos = ft_strstr(var, "+");
	char * const var_id = ft_substr(var, 0, plus_pos);
	const char *var_tail = &var[plus_pos + 2];
	if (!var_tail)
		return ;
	for (size_t i = 0; i < sl->size; i++) {
		const size_t equal_pos = ft_strstr(sl->data[i], "=");
		char * const curr_id = ft_substr(sl->data[i], 0, equal_pos);
		if (!ft_strcmp(curr_id, var_id)) {
			free(curr_id);
			free(var_id);
			char * const hold = sl->data[i];
			sl->data[i] = gc(GC_ADD, ft_strjoin(sl->data[i], var_tail), GC_ENV);
			gc(GC_FREE, hold, GC_ENV);
			return ;
		}
		free(curr_id);
	}
}

char *string_list_get_value(const StringListL * const sl, char * const id) {
	for (size_t i = 0; i < sl->size; i++) {
		const char *curr_equal_ptr = ft_strchr(sl->data[i], '=');
		const size_t curr_equal_pos = curr_equal_ptr - sl->data[i];
		char * const curr_id = ft_substr(sl->data[i], 0, curr_equal_pos);

		if  (!ft_strcmp(curr_id, id)) {
			free(curr_id);
			return &sl->data[i][curr_equal_pos + 1];
		}
		free(curr_id);
	}
	return NULL;
}

char *get_positional_value(const StringListL * const sl, const size_t idx) {
	 return (ft_strchr(sl->data[idx], '=') + 1);
}

void string_list_print(const StringListL *list) {
	for (size_t i = 0; i < list->size; i++) {
		const char *eql = ft_strchr(list->data[i], '=');
		if (!eql)
			continue;
		printf("%s\n", list->data[i]);
	}
}
