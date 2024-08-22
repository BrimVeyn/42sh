/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:13:40 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/22 10:33:47 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

char	*ft_strchr(const char *string, int searchedChar)
{
	char	c;

	c = (char)searchedChar;
	if (c == '\0')
		return (NULL);
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
