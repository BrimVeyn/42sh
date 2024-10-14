/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_find_first_of.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 14:47:45 by nbardavi          #+#    #+#             */
/*   Updated: 2024/05/24 15:32:49 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/c_string.h"
#include <string.h>

size_t str_find_first_of(const string *str, size_t c){
	for(size_t i = 0; i < str->size; i++){if((size_t)str->data[i] == c){return i;} }
	return str->size;
}
