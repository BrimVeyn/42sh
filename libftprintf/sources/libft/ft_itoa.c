/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/04 12:49:16 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 12:39:01 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

static int	count_digit(int n)
{
	int				cpt;
	unsigned int	nbr;

	cpt = 1;
	if (n < 0)
	{
		nbr = -n;
		cpt++;
	}
	else
		nbr = n;
	while (nbr >= 10)
	{
		nbr /= 10;
		cpt++;
	}
	return (cpt);
}

char	*ft_itoa(int n)
{
	char			*returnvalue;
	int				i;
	int				fin;
	unsigned int	nbr;

	i = count_digit(n) - 1;
	fin = count_digit(n);
	returnvalue = malloc(sizeof(char) * fin + 1);
	if (!returnvalue)
		return (NULL);
	if (n < 0)
	{
		nbr = -n;
		returnvalue[0] = '-';
	}
	else
		nbr = n;
	while (nbr > 9)
	{
		returnvalue[i--] = (nbr % 10) + '0';
		nbr /= 10;
	}
	returnvalue[i] = nbr + '0';
	returnvalue[fin] = '\0';
	return (returnvalue);
}
