/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StringStream.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 14:50:43 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/14 15:11:24 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "libft.h"

char string_stream_pop_front(StringStream *ss) {
    if (ss->size == 0)
        return 0;
    char first_element = ss->data[0];
    ft_memmove(&(ss->data[0]), &(ss->data[1]), ss->size_of_element * (ss->size - 1));
	ss->data[--ss->size] = 0;
    return first_element;
}
