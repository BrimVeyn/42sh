/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exec_d.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/16 13:33:14 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 14:26:25 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/ft_printf.h"

char	*exec_d(char *str, t_trigger trigger)
{
	if (trigger.dot == 1)
		str = fdot(str, trigger, ft_strlen(str));
	if (trigger.plus == 1)
		if (str[0] != '-')
			str = ft_strjoin_s2("+", str);
	if (trigger.space == 1 && trigger.number == 0 && trigger.plus == 0)
		if (str[0] != '-')
			str = ft_strjoin_s2(" ", str);
	if (trigger.number == 1)
		str = joinflag(0, str, ' ', trigger.number1 - ft_strlen(str));
	if (trigger.hyphen == 1)
		str = joinflag(1, str, ' ', trigger.number1 - ft_strlen(str));
	if (trigger.zero == 1)
		str = fzero(str, trigger, ft_strlen(str));
	return (str);
}

char	*exec_x(char *str, t_trigger trigger, char flag)
{
	if (trigger.dot == 1)
		str = fdot(str, trigger, ft_strlen(str));
	if (trigger.zero == 1)
		str = fzero(str, trigger, ft_strlen(str));
	if (trigger.hash == 1 && flag == 'x')
	{
		if (trigger.zero == 0 && trigger.origine != 0)
			str = ft_strjoin_s2("0x", str);
		else if (trigger.zero != 0 && trigger.origine != 0)
			str = ft_strjoin("0x", ft_substr(str, 2, ft_strlen(str)));
	}
	if (trigger.hash == 1 && flag == 'X')
	{
		if (trigger.zero == 0 && trigger.origine != 0)
			str = ft_strjoin_s2("0X", str);
		else if (trigger.zero != 0 && trigger.origine != 0)
			str = ft_strjoin("0X", ft_substr(str, 2, ft_strlen(str)));
	}
	if (trigger.number == 1)
		str = joinflag(0, str, ' ', trigger.number1 - ft_strlen(str));
	if (trigger.hyphen == 1)
		str = joinflag(1, str, ' ', trigger.number1 - ft_strlen(str));
	return (str);
}

char	*exec_s(char *str, t_trigger trigger)
{
	if (trigger.dot == 1)
		str = sdot(str, trigger);
	if (trigger.number == 1)
		str = joinflag(0, str, ' ', trigger.number1 - ft_strlen(str));
	if (trigger.hyphen == 1)
		str = joinflag(1, str, ' ', trigger.number1 - ft_strlen(str));
	if (trigger.fr == 1)
		str = ft_strdup(str);
	if (trigger.tr == 1)
		str = ft_substr_free(str, trigger.tr_nbr, ft_strlen(str));
	return (str);
}

char	*exec_p(char *str, t_trigger trigger)
{
	if (trigger.number == 1)
		str = joinflag(0, str, ' ', trigger.number1 - ft_strlen(str));
	if (trigger.hyphen == 1)
		str = joinflag(1, str, ' ', trigger.number1 - ft_strlen(str));
	return (str);
}

char	*exec_c(char *str, t_trigger trigger)
{
	if (str[0] != -9)
	{
		if (trigger.number == 1)
			str = joinflag(0, str, ' ', trigger.number1 - 1);
		if (trigger.hyphen == 1)
			str = joinflag(1, str, ' ', trigger.number1 - 1);
	}
	else
	{
		if (trigger.number == 1)
			str = joinflag(0, str, ' ', trigger.number1 - 1);
		else if (trigger.hyphen == 1)
			str = joinflag(1, str, ' ', trigger.number1 - 1);
	}
	return (str);
}
