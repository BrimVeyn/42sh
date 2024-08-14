/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 16:05:54 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 12:39:07 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*returnvalue;

	if (size == 0)
	{
		returnvalue = malloc(0);
		return (returnvalue);
	}
	if (nmemb <= 0 && size <= 0)
		return (NULL);
	if (nmemb > SIZE_MAX / size)
		return (NULL);
	returnvalue = malloc(nmemb * size);
	if (returnvalue == NULL || (!nmemb && !size))
		return (NULL);
	if (returnvalue)
	{
		ft_memset(returnvalue, 0, nmemb * size);
	}
	return (returnvalue);
}
