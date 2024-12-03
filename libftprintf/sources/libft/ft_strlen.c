/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 10:48:08 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/03 18:24:21 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"
#include <stddef.h>
#include <stdint.h>

size_t ft_strlen(const char *theString) {
    if (!theString) return 0;

    const char *s = theString;

    // Process word-sized chunks for performance
    const uintptr_t *word_ptr = (const uintptr_t *)s;
    while (1) {
        uintptr_t word = *word_ptr;

        // Check for null bytes in the word (use bitwise operations)
        if ((word - 0x0101010101010101UL) & ~word & 0x8080808080808080UL) {
            break;
        }
        word_ptr++;
    }

    // Locate the exact position of the null byte
    s = (const char *)word_ptr;
    while (*s) s++;

    return s - theString;
}
