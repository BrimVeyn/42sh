/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:13:40 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/30 00:39:49 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"
#include <stdio.h>

char	*ft_strchr(const char *string, int searchedChar)
{
	char	c;

	c = (char)searchedChar;
	if (c == '\0' || !string || !*string)
		return (NULL);
	while (*string)
	{
		if (*string == c)
			return ((char *)string);
		string++;
	}
	return (NULL);
}
