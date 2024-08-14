/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 13:58:16 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 12:37:58 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

void	*ft_memchr(const void *memoryBlock, int searchedChar, size_t size)
{
	unsigned char	searched;
	unsigned char	*block;

	block = (unsigned char *)memoryBlock;
	searched = (unsigned char)searchedChar;
	while (size-- > 0)
	{
		if (*block++ == searched)
			return ((void *)--block);
	}
	return (NULL);
}

/*
int	main(void)
{
	char data[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
	const unsigned int size = 10;

	// On recherche une valeur inhéxistante :
	void *found = ft_memchr(data, 57, size);
	printf("57 is %s\n", (found != NULL ? "found" : "not found"));

	// On recherche une valeur existante :
	found = ft_memchr(data, 50, size);
	printf("50 is %s\n", (found != NULL ? "found" : "not found"));
	if (found != NULL)
	{
		printf("La valeur à la position calculée est %d\n", *((char *)found));
	}

	return (EXIT_SUCCESS);
}*/