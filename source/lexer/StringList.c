/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringList.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 14:12:30 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/18 11:20:58 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

/**
 * @brief Extracts the name from an environment variable string.
 *
 * Copies the portion before the '=' into `str`.
 */
void get_env_variable_id(char *buffer, char *variable){
	int equal_pos = ft_strstr(variable, "=");
	ft_memcpy(buffer, variable, equal_pos * sizeof(char));
    buffer[equal_pos] = '\0';
}

/**
 * @brief Extracts the value from an environment variable string.
 *
 * Copies the portion after the '=' into `buffer`.
 */
void get_env_variable_value(char *buffer, char *variable){
	int equal_pos = ft_strstr(variable, "=");
	ft_memcpy(buffer, &variable[equal_pos + 1], (ft_strlen(variable) - equal_pos) * sizeof(char));
    buffer[ft_strlen(variable) - equal_pos - 1] = '\0';
}

int get_env_variable_index(StringList *sl, char *variable){
	int i = 0;

	char id[4096] = {0};
	get_env_variable_id(id, variable);

	for (; i < sl->size; i++){
		char tmp_id[POSIX_MAX_ID_LEN] = {0};
		get_env_variable_id(id, sl->value[i]);
		if (!ft_strcmp(id, tmp_id)){
			break;
		}
		ft_memset(tmp_id, 0, ft_strlen(tmp_id) * sizeof(char));
	}
	return (i == sl->size ? -1 : i);
}

StringList *string_list_init(void) {
	StringList *self = gc_add(ft_calloc(1, sizeof(StringList)), GC_SUBSHELL);
	StringList tl = {
		.value = (char **) gc_add(ft_calloc(10, sizeof(char *)), GC_SUBSHELL),
		.size = 0,
		.capacity = 10,
	};
	*self = tl;
	return self;
}

/**
 * @brief Adds or updates a variable in the string list.
 *
 * Resizes the list if necessary, then adds the variable or updates it if it exists.
 */
void string_list_add_or_update(StringList *sl, char *variable) {
	if (sl->size >= sl->capacity - 1) {
		sl->capacity *= 2;
		char **tmp_value = sl->value;
		sl->value = (char **)ft_realloc(sl->value, sl->size, sl->capacity, sizeof(char *));
		gc_free(tmp_value, GC_SUBSHELL);
		gc_add(sl->value, GC_SUBSHELL);
	}

	int index = get_env_variable_index(sl, variable);
	if (index != -1){
		sl->value[index] = variable;
		return;
	}

	sl->value[sl->size] = variable;
	sl->size += 1;
	sl->value[sl->size] = NULL;
}

void string_list_remove(StringList *sl, char *variable){
	int index = get_env_variable_index(sl, variable);
	
	if (index != -1){
		for (int j = index; j < sl->size - 1; j++) {
			sl->value[j] = sl->value[j + 1];
		}
		sl->value[sl->size - 1] = NULL;
		sl->size--;
	}
}

char *string_list_get_value_with_id(StringList *sl, char *id){
	uint16_t i = 0;
	for (; i < sl->size; i++){

		char tmp_id[POSIX_MAX_ID_LEN] = {0};
		get_env_variable_id(tmp_id, sl->value[i]);
		if (!ft_strcmp(id, tmp_id)){
			char *value = gc_add(ft_calloc(ft_strlen(sl->value[i]), sizeof(char)), GC_GENERAL);
			get_env_variable_value(value, sl->value[i]);
			return value;
		}
		ft_memset(tmp_id, 0, ft_strlen(tmp_id) * sizeof(char));
	}
	return NULL;
}
