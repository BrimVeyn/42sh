/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_stream.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 14:29:02 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/20 15:01:28 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "string_stream.h"
#include "lexer.h"
#include <stdint.h>

StringStream ss_init(void) {
	StringStream self;

	self.str = ft_strdup(NULL);
	self.size = 0;
	self.capacity = 10;
	return self;
}

void ss_append_string(StringStream *ss, char *str) {
	const uint16_t str_len = ft_strlen(str);

	if (ss->size + str_len >= ss->capacity) {
		ss->capacity *= 2;
		ss->str = (char *) ft_realloc(ss->str, ss->size, ss->capacity, sizeof(char));
	}
	ft_strlcat(ss->str, str, str_len);
	ss->size += str_len;
}

void ss_append_char(StringStream *ss, char c) {
	if (ss->size + 1 >= ss->capacity) {
		ss->capacity *= 2;
		ss->str = (char *) ft_realloc(ss->str, ss->size, ss->capacity, sizeof(char));
	}
	ft_strlcat(ss->str, &c, 1);
	ss->size += 1;
}

void ss_append_int(StringStream *ss, int n) {
	char *res = ft_itoa(n);
	const int16_t str_len = strlen(res);

	if (ss->size + str_len >= ss->capacity) {
		ss->capacity *= 2;
		ss->str = (char *) ft_realloc(ss->str, ss->size, ss->capacity, sizeof(char));
	}
	ft_strlcat(ss->str, res, str_len);
	ss->size += str_len;
	free(res);
}

char *ss_toOwnedSlice(StringStream *ss) {
	return ss->str;
}
