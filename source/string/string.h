/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 13:45:27 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/15 13:50:13 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

typedef struct String {
	char *str;
	size_t size;
	size_t capacity;
} String;

void string_append_char(String *s, char c);
void string_append_slice(String *s, char *c);
void string_append_string(String *lhs, String *rhs);
