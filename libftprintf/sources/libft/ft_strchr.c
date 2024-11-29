/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:13:40 by nbardavi          #+#    #+#             */
/*   Updated: 2024/11/29 14:51:04 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"
#include <stdio.h>

char	*ft_strchr(const char *string, int searchedChar)
{
	char	c;

	c = (char)searchedChar;
	if (c == '\0')
		return (NULL);
	// printf("string: %s\n", string);
	// fflush(stdout);
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
