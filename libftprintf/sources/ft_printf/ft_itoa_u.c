/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa_u.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 11:54:21 by bvan-pae          #+#    #+#             */
/*   Updated: 2023/12/24 12:34:31 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/ft_printf.h"
#include <unistd.h>

static int	count_digit(unsigned int nbr)
{
	int	cpt;

	cpt = 1;
	while (nbr >= 10)
	{
		nbr /= 10;
		cpt++;
	}
	return (cpt);
}

char	*ft_itoa_u(unsigned int nbr)
{
	char	*returnvalue;
	int		i;
	int		fin;

	i = count_digit(nbr) - 1;
	fin = i + 1;
	returnvalue = malloc(sizeof(char) * fin + 1);
	if (!returnvalue)
		return (NULL);
	while (nbr > 9)
	{
		returnvalue[i--] = (nbr % 10) + '0';
		nbr /= 10;
	}
	returnvalue[i] = nbr + '0';
	returnvalue[fin] = '\0';
	return (returnvalue);
}
