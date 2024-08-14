/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/04 13:28:01 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 12:38:01 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char	*returnvalue;
	int		i;

	if (!s)
		return (NULL);
	returnvalue = (char *)malloc((ft_strlen(s) + 1) * sizeof(char));
	if (returnvalue == NULL)
		return (NULL);
	i = 0;
	while (s[i])
	{
		returnvalue[i] = f(i, s[i]);
		i++;
	}
	returnvalue[i] = '\0';
	return (returnvalue);
}
