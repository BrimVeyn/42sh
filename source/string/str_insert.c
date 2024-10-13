/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_insert.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 15:00:30 by nbardavi          #+#    #+#             */
/*   Updated: 2024/05/24 15:21:07 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/c_string.h"
#include <string.h>

void str_insert(string *str, const char *str2, size_t i){
	size_t len = strlen(str2);
	str_assign_right_size(str, len + str->size);
	memmove(str->data + i + len, str->data + i, str -> size - i + 1);
	memcpy(str->data + i, str2, len);
	str->size += len;
}
