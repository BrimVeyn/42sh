/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_pop_front.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 14:56:30 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/11 11:17:12 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/c_string.h"
#include "../.././libftprintf/header/libft.h"
#include <string.h>

void str_pop_front(string *str){
	ft_memmove(str->data, str->data + 1, str->size + 1);
	str->size--;
}
