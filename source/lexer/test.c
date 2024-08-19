/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 15:06:02 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/19 15:39:56 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>

void ex1(int x) {
	printf("x = %d\n", x);
}

void ex2(float y) {
	printf("y = %f\n", y * 2);
}

#define exemple(param) _Generic((param), \
	int: ex1, \
	float: ex2, \
    default : printf("\n") \
)(param)

// int main(void) {
//
// 	int x = 12;
// 	float y = 12.5f;
//
// 	exemple(x);
// 	exemple(y);
// }


