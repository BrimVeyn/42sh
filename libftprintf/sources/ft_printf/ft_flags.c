/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_flags.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:03:01 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 14:33:24 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/ft_printf.h"

t_trigger	t_flags(const char *txt, int i)
{
	char		*stop;
	t_trigger	trigger;

	stop = "dicspuxX";
	trigger = reset_struct();
	while (ft_instr(txt[i], stop) == 0)
	{
		if (txt[i] == '-')
		{
			trigger.hyphen = 1;
			trigger = get_nbr(txt, ++i, trigger);
			i += trigger.index;
		}
		trigger = t_flags2(txt, i, trigger);
		i = trigger.index;
		trigger = t_flags3(txt, i, trigger);
		i = trigger.index;
		trigger = t_flags4(txt, i, trigger);
		i = trigger.index;
	}
	trigger.index = i;
	return (trigger);
}

t_trigger	t_flags2(const char *txt, int i, t_trigger trigger)
{
	if (txt[i] == '0')
	{
		trigger.zero = 1;
		trigger = get_nbr(txt, ++i, trigger);
		i += trigger.index;
	}
	else if (txt[i] == '.')
	{
		trigger.dot = 1;
		trigger = get_nbr(txt, ++i, trigger);
		i += trigger.index;
	}
	trigger.index = i;
	return (trigger);
}

t_trigger	t_flags3(const char *txt, int i, t_trigger trigger)
{
	if (txt[i] == '#')
	{
		trigger.hash = 1;
		i++;
	}
	else if (txt[i] == ' ')
	{
		trigger.space = 1;
		i++;
	}
	else if (txt[i] == '+')
	{
		trigger.plus = 1;
		i++;
	}
	trigger.index = i;
	return (trigger);
}

t_trigger	t_flags4(const char *txt, int i, t_trigger trigger)
{
	if (txt[i] == 'f')
	{
		trigger.fr = 1;
		i++;
	}
	if (txt[i] == 't')
	{
		trigger.tr = 1;
		trigger = get_nbr(txt, ++i, trigger);
		i += trigger.index;
	}
	if (txt[i] <= '9' && txt[i] >= '1')
	{
		trigger.number = 1;
		trigger = get_nbr(txt, i, trigger);
		i += trigger.index;
	}
	trigger.index = i;
	return (trigger);
}
