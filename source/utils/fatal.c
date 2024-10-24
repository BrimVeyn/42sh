/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fatal.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 11:44:58 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/13 11:46:44 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include <stdlib.h>

void fatal(char *msg, int exit_code) {
	ft_dprintf(2, "%s\n", msg);
	gc(GC_CLEANUP, GC_ALL);
	exit(exit_code);
}
