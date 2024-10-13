/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_pop_front.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 14:56:30 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/13 17:33:17 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "c_string.h"
#include "libft.h"

char str_pop_front(string *str){
	if (str->size == 0)
		return '\0';

	char c = str->data[0];
	ft_memmove(str->data, str->data + 1, str->size);
	str->size--;
	return c;
}
