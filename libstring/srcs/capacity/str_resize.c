/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_resize.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:21:23 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/11 11:13:48 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/c_string.h"
#include "../.././libftprintf/header/libft.h"
#include <stdlib.h>
#include <string.h> 

void str_resize(string *str, size_t size) {
	str_assign_right_size(str, size);
	if (size > str->size){
		ft_memset(str->data + str->size, ' ', size - str->size);
	}
	str->data[size] = '\0';
	str->size = size;
}
