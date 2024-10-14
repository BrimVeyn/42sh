/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_compare.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 09:33:59 by nbardavi          #+#    #+#             */
/*   Updated: 2024/05/27 14:11:46 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/c_string.h"
#include <string.h>
#include <immintrin.h>

int str_compare(char *s1, char *s2){
	return strcmp(s1, s2);
}

int str_compare_simd(string *s1, string *s2) {
    size_t i = 0;
	
	if (s1->size < 16 || s2->size < 16){
		return strcmp(s1->data, s2->data);
	}

    while (i + 16 <= s1->size && i + 16 <= s2->size) {
        __m128i chunk1 = _mm_loadu_si128((const __m128i *)(s1->data + i));
        __m128i chunk2 = _mm_loadu_si128((const __m128i *)(s2->data + i));
        __m128i result = _mm_cmpeq_epi8(chunk1, chunk2);
        int mask = _mm_movemask_epi8(result);

        if (mask != ~0) {
            int differing_index = __builtin_ctz(~mask);
            return (unsigned char)s1->data[i + differing_index] - (unsigned char)s2->data[i + differing_index];
        }
        i += 16;
    }
    while (i < s1->size && i < s2->size) {
        if (s1->data[i] != s2->data[i]) {
            return (unsigned char)s1->data[i] - (unsigned char)s2->data[i];
        }
        i++;
    }
    return (i < s1->size) ? (unsigned char)s1->data[i] : ((i < s2->size) ? -(unsigned char)s2->data[i] : 0);
}
