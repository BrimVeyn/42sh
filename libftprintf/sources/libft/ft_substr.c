/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 16:48:21 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 12:38:03 by nbardavi         ###   ########.fr       */
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

// int main(void)
// {
//   char *test = ft_substr("tripouille", 0, 42000);
//   printf("%s", test);
//   return (0);
// }
