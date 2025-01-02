/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 12:20:06 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/26 16:37:23 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "exec.h"
#include "ft_regex.h"
#include "utils.h"
#include "libft.h"

static int read_x_base(const char *const input, char *buffer, const int x, const char *const base) {
	int i = 0;

	while (i < x && ft_strchr(base, input[i])) {
		buffer[i] = ft_tolower(input[i]);
		i++;
	}
	buffer[i] = '\0';
	return i;
}

static char *unicode_to_uf8(uint64_t codepoint) {
	static char buffer[5] = {0};
	if (codepoint < 0x7F) {
		buffer[0] = codepoint;
		buffer[1] = '\0';
	} else if (codepoint <= 0x7FF) {
		buffer[0] = 0xC0 | ((codepoint >> 6) & 0x1F);
		buffer[1] = 0x80 | (codepoint & 0x3F);
		buffer[2] = '\0';
	} else if (codepoint <= 0xFFFF) {
        buffer[0] = 0xE0 | ((codepoint >> 12) & 0x0F);
        buffer[1] = 0x80 | ((codepoint >> 6) & 0x3F);
        buffer[2] = 0x80 | (codepoint & 0x3F);
        buffer[3] = '\0';
    } else if (codepoint <= 0x10FFFF) {
        buffer[0] = 0xF0 | ((codepoint >> 18) & 0x07);
        buffer[1] = 0x80 | ((codepoint >> 12) & 0x3F);
        buffer[2] = 0x80 | ((codepoint >> 6) & 0x3F);
        buffer[3] = 0x80 | (codepoint & 0x3F);
        buffer[4] = '\0';
    } else {
        return NULL;
    }
	return buffer;
}


char *replace_escape_sequences(char *arg, bool *stop) {
	
	unsigned char lookup[128] = {
		['\\'] = '\\', ['a'] = '\a', ['b'] = '\b',
		['c'] = EOF, ['e'] = '\033', ['f'] = '\f',
		['n'] = '\n', ['r'] = '\r', ['t'] = '\t', ['v'] = '\v',
	};

	// dprintf(2, "arg: %s\n", arg);
	da_create(ss, StringStream, sizeof(char), GC_SUBSHELL);

	for (size_t i = 0; arg[i]; i++) {
		if (arg[i] == '\\' && arg[i + 1]) {
			if (lookup[(size_t) arg[i + 1]] != 0) {
				if (arg[i + 1] == 'c') {
					return (*stop = true), ss_get_owned_slice(ss);
				}
				da_push(ss, lookup[(size_t) arg[++i]]);

			} else if (arg[i + 1] == '0') {
				i += 2; // '\' + '0';
				
				char buffer[4] = {0};
				//increment i of bytes read
				i += (read_x_base(&arg[i], buffer, 3, "01234567") - 1);

				da_push(ss, ft_atoi_base(buffer, "01234567"));

			} else if (arg[i + 1] == 'u' || arg[i + 1] == 'U') {
				//2 bytes utf if 'u' 4 bytes utf if 'U'
				int read_size = (arg[i + 1] == 'u') ? 4 : 8;
				i += 2; // '\' + 'u'

				char buffer[5] = {0};
				i += (read_x_base(&arg[i], buffer, read_size, "0123456789abcdefABCDEF") - 1);

				uint64_t ret = ft_atoi_base(buffer, "0123456789abcdef");
				// dprintf(2, "value: %ld\n", ret);
				char *unicode_str = unicode_to_uf8(ret);
				ss_push_string(ss, unicode_str);
			} else if (arg[i + 1] == 'x') {
				int read_size = 2;
				i += 2; // '\' + 'x'

				char buffer[3] = {0};
				i += (read_x_base(&arg[i], buffer, read_size, "0123456789abcdefABCDEF") - 1);
				uint64_t ret = ft_atoi_base(buffer, "0123456789abcdef");
				da_push(ss, ret);
			} else da_push(ss, arg[i])

		} else {
			da_push(ss, arg[i]);
		}
	}
	return ss_get_owned_slice(ss);
}

void builtin_echo(const SimpleCommandP *command, UNUSED Vars *const shell_vars) {

	enum EchoOptions {
		NO_NEWLINE = (1 << 0),
		ESCAPE_SEQ = (1 << 1),
	};

	char buffer[MAX_WORD_LEN] = {0};
	int options = 0;
	size_t i = 1;

	int buffer_size = 0;

	for (; command->word_list->data[i]; i++) {
		char *arg = command->word_list->data[i];

		if (regex_match("^-n+$", arg).is_found) {
			options |= NO_NEWLINE;
		} else if (regex_match("^-e+$", arg).is_found) {
			options |= ESCAPE_SEQ;
		} else break;
	}

	bool stop = false;

	for (; i < command->word_list->size; i++) {
		char *arg = command->word_list->data[i];
		char *output = (options & ESCAPE_SEQ) ? replace_escape_sequences(arg, &stop) : arg;

		if (!stop && command->word_list->data[i + 1]) {
			buffer_size = ft_snprintf(buffer, MAX_WORD_LEN, "%s ", output);
			if (buffer_size == -1)
				_fatal("snprintf: buffer overflow", 1);
		} else {
			buffer_size = ft_snprintf(buffer, MAX_WORD_LEN, "%s", output);
			if (buffer_size == -1)
				_fatal("snprintf: buffer overflow", 1);
		}
		if (stop)
			break;
	}

	if (!stop && (options & NO_NEWLINE) == 0) {
		buffer_size = ft_snprintf(buffer, MAX_WORD_LEN, "\n");
		if (buffer_size == -1)
			_fatal("snprintf: buffer overflow", 1);
	}

	if (write(STDOUT_FILENO, buffer, buffer_size) == -1)
		return error("42sh: echo: write error", 1);

	g_exitno = 0;
}
