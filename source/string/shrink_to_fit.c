/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shrink_to_fit.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:21:11 by nbardavi          #+#    #+#             */
/*   Updated: 2024/05/24 15:18:44 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/c_string.h"
#include <stdlib.h>

void str_shrink_to_fit(string *str){
	if (str->capacity != str->size + 1){
		str->data = realloc(str->data, str->size + 1);
		str->capacity = str->size + 1;
	}
}
