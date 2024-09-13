/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EnvList.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 14:12:30 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/13 14:31:05 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"


StringList *string_list_init(void) {
	StringList *self = gc_add(ft_calloc(1, sizeof(StringList)), GC_GENERAL);
	StringList tl = {
		.value = (char **) gc_add(ft_calloc(10, sizeof(char *)), GC_GENERAL),
		.id = (char **) gc_add(ft_calloc(10, sizeof(char *)), GC_GENERAL),
		.size = 0,
		.capacity = 10,
	};
	*self = tl;
	return self;
}

void string_list_add_or_update(StringList *sl, char *id, char *value) {
	if (sl->size >= sl->capacity) {
		sl->capacity *= 2;
		char **tmp_value = sl->value;
		char **tmp_id = sl->id;
		sl->value = ft_realloc(sl->value, sl->size, sl->capacity, sizeof(Token *));
		sl->id = ft_realloc(sl->value, sl->size, sl->capacity, sizeof(Token *));
		gc_free(tmp_value, GC_GENERAL);
		gc_free(tmp_id, GC_GENERAL);
		gc_add(sl->value, GC_GENERAL);
		gc_add(sl->id, GC_GENERAL);
	}
	
	for (uint16_t i = 0; i < sl->size; i++){
		if (!ft_strcmp(id, sl->id[i])){
			sl->value[i] = value;
			return;
		}
	}

	sl->value[sl->size] = value;
	sl->id[sl->size] = id;
	sl->size += 1;
}

void string_list_remove(StringList *sl, char *id){
	uint16_t i = 0;
	for (; i < sl->size; i++){
		if (!ft_strcmp(id, sl->id[i])){
			break;
		}
	}

    for (int j = i; j < sl->size - 1; j++) {
        sl->value[j] = sl->value[j + 1];
        sl->id[j] = sl->id[j + 1];
    }

    sl->value[sl->size - 1] = NULL;
    sl->id[sl->size - 1] = NULL;

    sl->size--;
}
