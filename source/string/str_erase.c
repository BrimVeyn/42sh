/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_erase.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 15:00:27 by nbardavi          #+#    #+#             */
/*   Updated: 2024/05/24 15:20:28 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/c_string.h"
#include <string.h>

void str_erase(string *str, size_t pos, size_t len){
	memmove(str->data + pos, str->data + pos + len, str->size - pos - len + 1);
	str->size -= len;
}
