/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/11 19:08:47 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/24 16:34:23 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>

int	is_space(char str)
{
	if ((str >= 9 && str <= 13) || str == 32)
		return (1);
	return (0);
}

int	check_base_validity(char *base)
{
	int	i;
	int	j;
	int	count;

	i = 0;
	while (base[i])
	{
		j = 0;
		count = 0;
		while (base[j])
		{
			if (base[i] == base[j])
			{
				count++;
			}
			if (count >= 2 || base[i] == '-' || base[i] == '+'
				|| is_space(base[i]) == 1)
				return (0);
		j++;
		}
	i++;
	}
	if (i == 0 || i == 1)
		return (0);
	return (1);
}

int	ft_strstrc(char str, char *base)
{
	int	i;

	i = 0;
	while (base[i])
	{
		if (str == base[i])
			return (1);
	i++;
	}
	return (0);
}

int	pos_in_str(char *base, char str)
{
	int	i;

	i = 0;
	while (base[i])
	{
		if (base[i] == str)
			return (i);
	i++;
	}
	return (0);
}

int	ft_atoi_base(char *str, char *base)
{
	int				num;
	int				neg;
	int				i;
	unsigned int	len_base;

	len_base = 0;
	i = 0;
	neg = 1;
	num = 0;
	if (check_base_validity(base) == 0 || str[i] == 0)
		return (0);
	while (base[len_base])
		len_base++;
	while (str[i] == '+' || str[i] == '-' || is_space(str[i]) == 1)
	{
		if (str[i] == '-')
			neg *= -1;
		i++;
	}
	while (ft_strstrc(str[i], base) == 1)
	{
		num = num * len_base + pos_in_str(base, str[i]);
		i++;
	}
	return (num * neg);
}
