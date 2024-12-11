/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_shrink_to_fit.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:21:11 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/11 11:22:18 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/c_string.h"
#include "../.././libftprintf/header/libft.h"
#include <stdlib.h>

void str_shrink_to_fit(string *str){
	if (str->capacity != str->size + 1){
		char * tmp = ft_calloc(str->size + 1, sizeof(char));
		ft_memmove(tmp, str->data, str->size + 1);
		free(str->data);
		str->data = tmp;
		str->capacity = str->size + 1;
	}
}
