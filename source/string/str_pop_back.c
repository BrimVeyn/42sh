/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_pop_back.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 14:56:27 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/13 16:14:56 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "c_string.h"

char str_pop_back(string *str) {
	if (str->size == 0) return '\0';
	return str->data[--str->size];
}
