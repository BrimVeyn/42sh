/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_get.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 17:26:36 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/13 17:30:56 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "c_string.h"
#include "libft.h"

char *str_get(string *str) {
	str_push_back(str, 0);
	return ft_strdup(str->data);
}
