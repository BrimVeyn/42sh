/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_assign_right_size.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 15:15:37 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/24 14:24:28 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/c_string.h"
#include "../../include/utils.h"

//ATTENTION CROTTE
void str_assign_right_size(string *str, size_t len){
	while(str->capacity < len + 1){
		str->data = gc(GC_REALLOC, str->data, str->capacity, str->capacity << 2, sizeof(char), GC_READLINE);
		str->capacity = str->capacity << 1;
	}
}
