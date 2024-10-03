/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 16:48:21 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/03 13:47:21 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*returnvalue;
	size_t	i;

	i = 0;
	if (start >= ft_strlen(s))
		return (ft_strdup(""));
	if (len > ft_strlen(s) - start)
		len = ft_strlen(s) - start;
	returnvalue = malloc(sizeof(char) * len + 1);
	if (returnvalue == NULL)
		return (NULL);
	while (i < len)
		returnvalue[i++] = s[start++];
	returnvalue[i] = '\0';
	return (returnvalue);
}
