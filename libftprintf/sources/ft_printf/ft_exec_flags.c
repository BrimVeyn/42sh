/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exec_flags.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/16 15:04:47 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 14:35:12 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/ft_printf.h"

char	*exec_flags(const char *txt, int i, va_list args)
{
	t_trigger	trigger;

	trigger = t_flags(txt, i);
	i = trigger.index;
	if (txt[i] == 'd' || txt[i] == 'i')
	{
		trigger.origine = va_arg(args, int);
		return (exec_d(ft_itoa(trigger.origine), trigger));
	}
	if (txt[i] == 'c')
		return (exec_c(itc(va_arg(args, int)), trigger));
	if (txt[i] == 's')
	{
		trigger.s_origine = va_arg(args, char *);
		return (exec_s(trigger.s_origine, trigger));
	}
	if (txt[i] == 'p' || txt[i] == 'u' || txt[i] == 'x' || txt[i] == 'X')
		return (exec_flags2(txt, i, args, trigger));
	return (NULL);
}

char	*exec_flags2(const char *txt, int i, va_list args, t_trigger trigger)
{
	if (txt[i] == 'p')
		return (exec_p(to_hexa_p(va_arg(args, unsigned long long)), trigger));
	if (txt[i] == 'u')
	{
		trigger.origine = va_arg(args, unsigned int);
		return (exec_d(ft_itoa_u(trigger.origine), trigger));
	}
	if (txt[i] == 'x' || txt[i] == 'X')
	{
		trigger.origine = va_arg(args, unsigned int);
		return (exec_x(to_hexa(trigger.origine, txt[i]), trigger, txt[i]));
	}
	return (NULL);
}
