/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_files.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 11:19:10 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/11 11:23:09 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "libft.h"
#include "exec.h"

#include <stdlib.h>
#include <unistd.h>


char *get_line_x(char *in, const size_t n) {
    if (!in || n == 0) return NULL;

    char *line_ptr = in;
    
    for (size_t i = 0; i < n - 1; i++) {
        line_ptr = ft_strchr(line_ptr, '\n');
        if (!line_ptr) {
			return NULL;
		}
        line_ptr++;
    }

    char *end = ft_strchr(line_ptr, '\n');
    if (!end) {
		end = line_ptr + strlen(line_ptr);  // If no newline, go to end of string
	}

    return ft_substr(in, (line_ptr - in), (end - line_ptr));
}

char *read_whole_file(int fd) {
	char buffer[BUFFER_SIZE] = {0};

	size_t result_size = 0;
	size_t result_capacity = BUFFER_SIZE;
	size_t bytes_read = 0;
	char *result = ft_calloc(BUFFER_SIZE, sizeof(char));
	if (!result) {
		_fatal("read error", 1);
	}

	while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) != 0) {
		buffer[bytes_read] = '\0';
		if (result_size + bytes_read >= result_capacity) {
			result_capacity *= 2;
			char *tmp = result;
			result = ft_realloc(result, result_size, result_capacity, sizeof(char));
			free(tmp);
		}
		ft_memcpy(result + result_size, buffer, bytes_read);
		result_size += bytes_read;
		result[result_size] = '\0';
	}
	if (result_size != 0 && result[result_size - 1] == '\n')
		result[result_size - 1] = '\0';
	return result;
}
