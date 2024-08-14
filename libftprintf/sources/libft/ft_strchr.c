/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:13:40 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 12:38:00 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

char	*ft_strchr(const char *string, int searchedChar)
{
	char	c;

	c = (char)searchedChar;
	if (c == '\0')
		return ((char *)string + ft_strlen(string));
	while (*string)
	{
		if (*string == c)
			return ((char *)string);
		string++;
	}
	return (NULL);
}
//
// int main(void)
// {
//   printf("strchr: %s\n", strchr("tripouille", 't' + 256));
//   printf("ft_strchr: %s", ft_strchr("tripouille", 't' + 256));
//   return (0);
// }
