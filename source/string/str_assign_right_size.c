/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_assign_right_size.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 15:15:37 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/13 17:38:56 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/c_string.h"
#include <stdlib.h>

void str_assign_right_size(string *str, size_t len){
	while(str->capacity < len + 1){
		str->data = realloc(str->data, str->capacity << 2);
		str->capacity = str->capacity << 1;
	}
}
