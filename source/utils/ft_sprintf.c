/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_sprintf.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/14 22:18:12 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/02 17:20:33 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "libft.h"

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

static size_t ft_ltoa_buffer(char *buffer, long number) {
	size_t buffer_len = ft_strlen(buffer);
	size_t i = 0;
	bool neg = number < 0;

	if (!number) {
		buffer[buffer_len++] = '0';
		return 1;
	}
		
	if (neg) number = -number;

	while (number) {
		buffer[buffer_len++] = (number % 10) + '0';
		number = number / 10;
		i += 1;
	}

	if (neg) {
		buffer[buffer_len++] = '-';
		i += 1;
    }

	for (size_t j = 0; j < i / 2; j++) {
		char tmp = buffer[buffer_len - j - 1];
		buffer[buffer_len - j - 1] = buffer[buffer_len - i + j];
		buffer[buffer_len - i + j] = tmp;
	}

	return i;
}

void ft_dprintf(int fd, char *fmt, ...){
	va_list			args;
	va_start(args, fmt);

	const size_t fmt_lem = ft_strlen(fmt);

	for (size_t i = 0; i < fmt_lem; i++) {
		if (!ft_strncmp(&fmt[i], "%s", 2)) {
			const char *str = va_arg(args, char *);
			ft_putstr_fd(str, fd);
			i += 2;
		}
		if (!ft_strncmp(&fmt[i], "%d", 2)) {
			const int number = va_arg(args, int);
			ft_putnbr_fd(number, fd);
			i += 2;
		}
		write(fd, &fmt[i], 1);
	}
	va_end(args);
}

void ft_sprintf(char *buffer, char *fmt, ...) {
	va_list			args;
	va_start(args, fmt);

	const size_t fmt_lem = ft_strlen(fmt);

	size_t buffer_len = ft_strlen(buffer);

	for (size_t i = 0; i < fmt_lem;) {
		if (!ft_strncmp(&fmt[i], "%s", 2)) {
			const char *str = va_arg(args, char *);
			const size_t str_len = ft_strlen(str);
			ft_memcpy(buffer + buffer_len, str, str_len + 1);
			buffer_len += str_len;
			i += 2;
			continue;
		}
		if (!ft_strncmp(&fmt[i], "%d", 2)) {
			const int number = va_arg(args, int);
			const size_t number_len = (int) ft_ltoa_buffer(buffer, number);
			buffer_len += number_len;
			i += 2;
			continue;
		}
		if (!ft_strncmp(&fmt[i], "%ld", 3)) {
			const long number = va_arg(args, int);
			const size_t number_len = ft_ltoa_buffer(buffer, number);
			buffer_len += number_len;
			i += 3;
			continue;
		}
		buffer[buffer_len] = fmt[i];
		buffer_len += 1;
		i += 1;
	}
	buffer[buffer_len] = '\0';
	va_end(args);
}
