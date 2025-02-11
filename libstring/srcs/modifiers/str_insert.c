/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_insert.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 15:00:30 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/11 11:15:16 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/c_string.h"
#include <string.h>
#include "../.././libftprintf/header/libft.h"

void str_insert(string *str, const char c, size_t i){
	str_assign_right_size(str, 1 + str->size);
	ft_memmove(str->data + i + 1, str->data + i, str -> size - i + 1);
	ft_memcpy(str->data + i, &c, 1);
	str->size++;
}

void str_insert_str(string *str, const char *str2, size_t i){
	size_t len = strlen(str2);
	str_assign_right_size(str, len + str->size);
	ft_memmove(str->data + i + len, str->data + i, str -> size - i + 1);
	ft_memcpy(str->data + i, str2, len);
	str->size += len;
}
