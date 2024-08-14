/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 14:27:08 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 12:37:59 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

int	ft_memcmp(const void *pointer1, const void *pointer2, size_t size)
{
	unsigned char	*p1;
	unsigned char	*p2;

	p1 = (unsigned char *)pointer1;
	p2 = (unsigned char *)pointer2;
	while (size > 0 && *p1 == *p2)
	{
		p1++;
		p2++;
		size--;
	}
	if (size == 0)
		return (0);
	return (*p1 - *p2);
}

/*
int	main(void) {
	char str1[] = "Hello, World!";
	char str2[] = "Hello, World!";

	size_t len = sizeof(str1);

	int result_memcmp = memcmp(str1, str2, len);
	int result_ft_memcmp = ft_memcmp(str1, str2, len);

	if (result_memcmp == 0) {
		printf("memcmp: Les deux tableaux sont égaux.\n");
	} else {
		printf("memcmp: Les deux tableaux ne sont pas égaux.\n");
	}

	if (result_ft_memcmp == 0) {
		printf("ft_memcmp: Les deux tableaux sont égaux.\n");
	} else {
		printf("ft_memcmp: Les deux tableaux ne sont pas égaux.\n");
	}

	return (0);
}
*/
