/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringStream.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 14:50:43 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/10 11:29:56 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "libft.h"

char ss_pop_front(StringStream * const ss) {
    if (ss->size == 0)
        return 0;
    char first_element = ss->data[0];
    ft_memmove(&(ss->data[0]), &(ss->data[1]), ss->size_of_element * (ss->size - 1));
	ss->data[--ss->size] = 0;
    return first_element;
}

char *ss_get_owned_slice(StringStream * const ss) {
	da_push(ss, '\0');
	char *str = ss->data;
	ss->data = gc(GC_CALLOC, ss->capacity, sizeof(char), GC_SUBSHELL);
	ss->size = 0;
	return str;
}

char *ss_to_string(StringStream * const ss) {
	da_push(ss, '\0');
	char *string = gc(GC_ADD, ft_strdup(ss->data), ss->gc_level);
	da_pop(ss);
	return string;
}

void ss_push_string(StringStream * const ss, const char * const str) {
	if (!str)
		return ;
	for (size_t i = 0; str[i]; i++) {
		da_push(ss, str[i]);
	}
}

void ss_cut(StringStream * const ss, const size_t new_size) {
	ft_memset(&ss->data[new_size], 0, ss->size - new_size);
	ss->size = new_size;
}

void ss_insert_string(StringStream * const ss, const char * const str, const size_t pos) {
	if (!str) 
		return ;
	const size_t str_len = ft_strlen(str);
	const size_t new_size = ss->size + str_len;

	if (new_size >= ss->capacity) {
		ss->data = gc(GC_REALLOC, ss->data, ss->size, new_size + ss->capacity, ss->size_of_element, ss->gc_level);
		ss->capacity = new_size + ss->capacity;
	}

	ft_memmove(&ss->data[pos + str_len], &ss->data[pos], ss->size - pos);
	ft_memcpy(&ss->data[pos], str, str_len);
	ss->size = new_size;
}
