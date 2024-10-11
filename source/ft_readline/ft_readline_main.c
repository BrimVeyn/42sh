/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_readline_main.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 11:29:59 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/11 16:19:36 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_readline.h"
#include <stdio.h>

int main(void){
	while(1){
		char *input = ft_readline("42sh> ");
		if (!input){
			break;
		}
		// printf("input: %s", input);
		if (input[0])
			add_history(input);
	}
	for (int i = 0; i < history->length; i++){
		printf("history %d: %s", i, history->entries[i]->line.data);
	}
	ft_readline_clean();
	return 0;
}
