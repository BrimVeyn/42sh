/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_fcustoms.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/14 15:28:43 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 12:34:30 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/ft_printf.h"

char	*ft_strdup_char(char c)
{
	char	*returnvalue;

	returnvalue = ft_calloc(2, sizeof(char));
	returnvalue[0] = c;
	return (returnvalue);
}

char	*ft_strjoin_s2(char const *s1, char const *s2)
{
	char	*returnvalue;
	int		i;
	int		j;

	j = 0;
	i = 0;
	returnvalue = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (returnvalue == NULL)
		return (NULL);
	while (s1[j])
		returnvalue[i++] = (char)s1[j++];
	j = 0;
	while (s2[j])
		returnvalue[i++] = (char)s2[j++];
	returnvalue[i] = '\0';
	free((char *)s2);
	return (returnvalue);
}

char	*ft_substr_free(char const *s, unsigned int start, size_t len)
{
	char	*returnvalue;
	size_t	i;

	i = 0;
	if (start >= ft_strlen(s))
	{
		free((char *)s);
		return (ft_strdup(""));
	}
	if (len > ft_strlen(s) - start)
		len = ft_strlen(s) - start;
	returnvalue = malloc(sizeof(char) * len + 1);
	if (returnvalue == NULL)
		return (NULL);
	while (i < len)
		returnvalue[i++] = s[start++];
	returnvalue[i] = '\0';
	free((char *)s);
	return (returnvalue);
}

char	*ft_strdup_free(const char *source)
{
	char	*str2;
	int		i;

	i = -1;
	str2 = malloc(sizeof(char) * (ft_strlen(source) + 1));
	if (str2 == NULL)
	{
		free((void *)source);
		return (NULL);
	}
	while (source[++i])
		str2[i] = source[i];
	str2[i] = '\0';
	free((void *)source);
	return (str2);
}

char	*joinflag(int pos, char *str, char value, int taille)
{
	char	*returnstr;
	char	*temp;

	if (taille <= 0)
		return (str);
	returnstr = malloc(sizeof(char) * (taille + 1));
	ft_memset(returnstr, value, taille);
	returnstr[taille] = '\0';
	if (pos == 0)
	{
		temp = ft_strjoin(returnstr, str);
		free(returnstr);
		free(str);
		return (temp);
	}
	temp = ft_strjoin(str, returnstr);
	free(returnstr);
	free(str);
	return (temp);
}
