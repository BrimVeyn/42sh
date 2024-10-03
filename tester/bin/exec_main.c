/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_main.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/29 13:01:15 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/02 12:55:19 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "utils.h"

#include <stdio.h>

int main(void){
	gc_init(GC_GENERAL);
	printf("%s\n", find_bin_location("ls", NULL));
	gc(GC_CLEANUP, GC_GENERAL);
}
