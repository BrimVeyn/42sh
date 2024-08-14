/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 17:29:48 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 12:38:01 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
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
	return (returnvalue);
}

/*
int	main(void) {
	const char *str1 = "Hello, ";
	const char *str2 = "world!";
	char *result;

	result = ft_strjoin(str1, str2);

	if (result) {
		printf("Concatenated string: %s\n", result);
		free(result);
			// N'oubliez pas de libérer la mémoire allouée par ft_strjoin.
	} else {
		printf("Memory allocation error\n");
	}

	return (0);
}*/
