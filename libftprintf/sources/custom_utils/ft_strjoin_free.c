/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin_free.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 16:01:03 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 12:23:59 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/ft_printf.h"

char	*ft_strjoin_free(char const *s1, char const *s2)
{
	char	*cat;
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	cat = (char *)malloc((ft_strlen(s1) + ft_strlen(s2) + 1) * sizeof(char));
	if (cat == NULL)
		return (NULL);
	while (s1[i])
	{
		cat[i] = s1[i];
		i++;
	}
	while (s2[j])
	{
		cat[i + j] = s2[j];
		j++;
	}
	cat[i + j] = 0;
	free((void *)s1);
	free((void *)s2);
	return (cat);
}

// char	*ft_substr_free(char const *s, unsigned int start, size_t len)
// {
// 	char	*new;
// 	size_t	i;
// 	size_t	slen;
//
// 	i = 0;
// 	slen = ft_strlen(s);
// 	if (!s)
// 		return (NULL);
// 	if (start >= slen || len == 0)
// 		return (NULL);
// 	if (len > slen - start)
// 		len = slen - start;
// 	new = malloc((len + 1) * sizeof(char));
// 	if (new == NULL)
// 		return (NULL);
// 	while (i < len && s[start + i])
// 	{
// 		new[i] = s[start + i];
// 		i++;
// 	}
// 	new[i] = 0;
// 	free((void*)s);
// 	return (new);
// }
