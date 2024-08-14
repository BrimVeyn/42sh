/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 22:09:37 by nbardavi          #+#    #+#             */
/*   Updated: 2023/11/14 11:52:53 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_atoi(const char *theString)
{
	int	i;
	int	sign;
	int	result;

	result = 0;
	sign = 1;
	i = 0;
	while ((theString[i] >= 9 && theString[i] <= 13) || theString[i] == 32)
		i++;
	if (theString[i] == '+' && theString[i + 1] != '-')
		i++;
	if (theString[i] == '-')
	{
		i++;
		sign = -1;
	}
	while (theString[i] && theString[i] >= '0' && theString[i] <= '9')
	{
		result *= 10;
		result += theString[i] - '0';
		i++;
	}
	return (result * sign);
}
