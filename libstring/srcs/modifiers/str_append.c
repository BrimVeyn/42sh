/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_append.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 12:02:42 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/11 11:15:33 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/c_string.h"
#include "../.././libftprintf/header/libft.h"
#include <stdlib.h>
#include <string.h>

/**
 * Appends str2 to str1.
 */
void str_append(string *str1, string *str2){
	str_assign_right_size(str1, str1->size + str2->size + 1);
	ft_memcpy(str1->data + str1->size, str2->data, str2->size + 1);
	str1->size += str2->size;
}
