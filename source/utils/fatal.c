/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fatal.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 11:44:58 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/23 22:02:55 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include <stdlib.h>

void fatal(const char * const msg, const int exit_code) {
	ft_dprintf(2, "Fatal: %s\n", msg);
	gc(GC_CLEANUP, GC_ALL);
	exit(exit_code);
}

void error(const char * const msg, const int exit_code) {
	ft_dprintf(2, "%s\n", msg);
	g_exitno = exit_code;
}
