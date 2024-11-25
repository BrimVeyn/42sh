/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringStream.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 14:50:43 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/25 17:13:07 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "libft.h"

char string_stream_pop_front(StringStream *ss) {
    if (ss->size == 0)
        return 0;
    char first_element = ss->data[0];
    ft_memmove(&(ss->data[0]), &(ss->data[1]), ss->size_of_element * (ss->size - 1));
	ss->data[--ss->size] = 0;
    return first_element;
}

char *ss_get_owned_slice(StringStream *ss) {
	da_push(ss, '\0');
	char *str = ss->data;
	ss->data = gc(GC_CALLOC, ss->capacity, sizeof(char), GC_SUBSHELL);
	ss->size = 0;
	return str;
}

void ss_push_string(StringStream *ss, char *str) {
	for (size_t i = 0; str[i]; i++) {
		da_push(ss, str[i]);
	}
}

void ss_cut(StringStream *ss, size_t new_size) {
	ft_memset(&ss->data[new_size], 0, ss->size - new_size);
	ss->size = new_size;
}
