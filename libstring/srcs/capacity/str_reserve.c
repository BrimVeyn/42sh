/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reserve.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:22:07 by nbardavi          #+#    #+#             */
/*   Updated: 2024/05/24 15:18:44 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/c_string.h"
#include <stdlib.h>

void str_reserve(string *str, size_t reserve){
    if (reserve > str->capacity){
        str->data = realloc(str->data, reserve);
		str->capacity = reserve;
    }
}
