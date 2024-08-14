/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tohexa.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 13:50:49 by bvan-pae          #+#    #+#             */
/*   Updated: 2023/12/24 12:34:32 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/ft_printf.h"
#include <stdlib.h>

int	count_digit_hx(unsigned int nbr)
{
	int	cpt;

	cpt = 1;
	while (nbr > 15)
	{
		cpt++;
		nbr /= 16;
	}
	return (cpt);
}

char	*to_hexa(unsigned int nbr, char x)
{
	char	*base;
	char	*result;
	int		i;

	i = count_digit_hx(nbr);
	result = malloc((sizeof(char) * i + 1));
	if (result == NULL)
		return (NULL);
	result[i] = '\0';
	if (x == 'x')
		base = "0123456789abcdef";
	else
		base = "0123456789ABCDEF";
	i--;
	while (nbr > 15)
	{
		result[i--] = base[nbr % 16];
		nbr /= 16;
	}
	result[i] = base[nbr];
	return (result);
}

int	count_digit_hx_p(unsigned long long nbr)
{
	int	cpt;

	cpt = 1;
	while (nbr > 15)
	{
		cpt++;
		nbr /= 16;
	}
	return (cpt);
}

char	*to_hexa_p(unsigned long long nbr)
{
	char	*base;
	char	*result;
	int		i;
	char	*final;

	if (nbr == 0)
		return (ft_strdup("(nil)"));
	i = count_digit_hx_p(nbr);
	result = malloc((sizeof(char) * i + 1));
	result[i] = '\0';
	base = "0123456789abcdef";
	i--;
	while (nbr > 15)
	{
		result[i--] = base[nbr % 16];
		nbr /= 16;
	}
	result[i] = base[nbr];
	final = ft_strjoin("0x", result);
	free(result);
	return (final);
}
