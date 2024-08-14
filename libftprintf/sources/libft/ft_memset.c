/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 15:24:04 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 12:37:59 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

void	*ft_memset(void *pointer, int value, size_t count)
{
	unsigned char	*intptr;
	unsigned char	bytevalue;

	bytevalue = (unsigned char)value;
	intptr = (unsigned char *)pointer;
	while (count > 0)
	{
		*intptr = bytevalue;
		intptr++;
		count--;
	}
	return (pointer);
}
