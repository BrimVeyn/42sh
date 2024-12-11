/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_reserve.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:22:07 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/11 11:21:16 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/c_string.h"
#include <stdlib.h>
#include "../.././libftprintf/header/libft.h"

void str_reserve(string *str, size_t reserve){
    if (reserve > str->capacity){
		char * tmp = ft_calloc(reserve, sizeof(char));
		ft_memmove(tmp, str->data, str->size + 1);
		free(str->data);
		str->data = tmp;
		str->capacity = reserve;
    }
}
