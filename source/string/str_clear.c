/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_clear.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 11:21:25 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/13 16:57:16 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/c_string.h"
#include <string.h>

void str_clear(string *str){
	memset(str->data, 0, str->size);
	str->size = 0;
}
