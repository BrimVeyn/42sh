/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_files.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/14 16:35:28 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/18 11:03:53 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "libft.h"
#include "exec.h"

#include <stdlib.h>
#include <unistd.h>

char *get_next_line(int fd) {
	char buffer[2];
	char *line = NULL;
	size_t len = 0;

	buffer[1] = '\0';

	while (read(fd, buffer, 1) > 0 && buffer[0] != '\n') {
		char *tmp = realloc(line, len + 2);
		if (!tmp) {
			free(line);
			return NULL;
		}
		line = tmp;
		line[len] = buffer[0];
		len++;
	}
	if (line) {
		line[len] = '\0';
	}
	return line;
}

char *read_whole_file(int fd) {
	char buffer[BUFFER_SIZE] = {0};

	size_t result_size = 0;
	size_t result_capacity = BUFFER_SIZE;
	size_t bytes_read = 0;
	char *result = ft_calloc(BUFFER_SIZE, sizeof(char));
	if (!result) {
		gc_cleanup(GC_ALL);
		close_all_fds();
		close_std_fds();
		exit(EXIT_FAILURE);
	}

	while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) != 0) {
		// dprintf(2, "salut !\n");
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
