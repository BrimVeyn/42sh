/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringList.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 14:12:30 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/01 15:32:35 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/lexer.h"
#include "../../include/utils.h"
#include "../../libftprintf/header/libft.h"
#include <stdio.h>

StringList *string_list_init(void) {
	StringList *self = gc_add(ft_calloc(1, sizeof(StringList)), GC_SUBSHELL);
	StringList tl = {
		.data = (char **) gc_add(ft_calloc(10, sizeof(char *)), GC_SUBSHELL),
		.size = 0,
		.capacity = 10,
	};
	*self = tl;
	return self;
}

void string_list_add(StringList *tl, char *token) {
	if (tl->size >= tl->capacity) {
		tl->capacity *= 2;
		char **tmp = tl->data;
		tl->data = ft_realloc(tl->data, tl->size, tl->capacity, sizeof(char *));
		gc_free(tmp, GC_SUBSHELL);
		gc_add(tl->data, GC_SUBSHELL);
	}
	tl->data[tl->size] = token;
	tl->size += 1;
}

void string_list_add_or_update(StringList *sl, char *var) {
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
				gc_free(sl->data[i], GC_GENERAL);
				sl->data[i] = gc_add(ft_strdup(var), GC_GENERAL);
            }
			FREE_POINTERS(curr_id, id);
			return ;
		}
		free(curr_id);
	}
	free(id);
	string_list_add(sl, var);
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
		dprintf(2, "%s\n", list->data[i]);
	}
}
