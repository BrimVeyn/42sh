/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_substr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 09:18:06 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/09 10:17:14 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "c_string.h"

string str_substr(string *str, size_t pos, size_t len){
	if (pos >= str->size || len == 0 || len - pos > str->size){
		return string_init_str("");
	}
	string newStr = string_init_str("");
	memcpy(newStr.data, str->data + pos, len);
	newStr.data[len] = '\0';
	return newStr;
}
