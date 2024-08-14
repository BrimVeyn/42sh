/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_litoa.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 13:50:32 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/02/08 13:55:46 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

static long	count_digit(long n)
{
	int				cpt;
	unsigned long	nbr;

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

char	*ft_litoa(long n)
{
	char			*returnvalue;
	int				i;
	int				fin;
	unsigned long	nbr;

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
