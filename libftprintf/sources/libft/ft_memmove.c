/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 12:28:09 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 12:38:38 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

void	*ft_memmove(void *destination, const void *source, size_t n)
{
	unsigned char		*dest;
	const unsigned char	*src;

	src = (const unsigned char *)source;
	dest = (unsigned char *)destination;
	if (dest == src)
		return (destination);
	if (dest < src)
	{
		while (n--)
			*dest++ = *src++;
	}
	else
	{
		dest += n;
		src += n;
		while (n--)
			*(--dest) = *(--src);
	}
	return (destination);
}
