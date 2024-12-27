#include "tester.h"

#include <string.h>
#include <stdlib.h>

int ft_strstr(const char *const haystack, const char *const needle) {
	if (!haystack || !needle) {
		return -1;
	}
	
	const int needle_len = strlen(needle);
	int i = 0;

	while (haystack[i]) {
		if (!strncmp(&haystack[i], needle, needle_len)) {
			return i;
		}
		i += 1;
	}
	return -1;
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*returnvalue;
	size_t	i;

	i = 0;
	if (start >= strlen(s))
		return (strdup(""));
	if (len > strlen(s) - start)
		len = strlen(s) - start;
	returnvalue = malloc(sizeof(char) * len + 1);
	if (returnvalue == NULL)
		return (NULL);
	while (i < len)
		returnvalue[i++] = s[start++];
	returnvalue[i] = '\0';
	return (returnvalue);
}

#define BUFFER_SIZE 4096

char *read_whole_file(int fd) {
	char buffer[BUFFER_SIZE] = {0};

	size_t result_size = 0;
	size_t result_capacity = BUFFER_SIZE;
	size_t bytes_read = 0;
	char *result = calloc(BUFFER_SIZE, sizeof(char));

	while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) != 0) {
		buffer[bytes_read] = '\0';
		if (result_size + bytes_read >= result_capacity) {
			result_capacity *= 2;
			result = realloc(result, result_capacity * sizeof(char));
		}
		memcpy(result + result_size, buffer, bytes_read);
		result_size += bytes_read;
		result[result_size] = '\0';
	}
	if (result_size != 0 && result[result_size - 1] == '\n')
		result[result_size - 1] = '\0';
	return result;
}
