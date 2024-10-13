/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_init_str.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 12:27:33 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/13 16:08:16 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "c_string.h"

#include <string.h>

string string_init_str(char *str) {
	string ptr;

	ptr.data = strdup(str);
	ptr.size = strlen(ptr.data);
	ptr.capacity = ptr.size + 1;
	return ptr;
}
