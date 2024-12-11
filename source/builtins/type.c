/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   type.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 11:02:12 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/11 10:50:44 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "exec.h"
#include "libft.h"
#include "parser.h"
#include "utils.h"

#define TYPE_BIN_NOT_FOUND(bin) ft_dprintf(2, "42sh: type: %s: not found\n", bin);

static bool is_keyword(const char *arg) {
	static const char *keywords[] = {
		"if", "then", "else", "elif", "fi", "case",
		"esac", "for", "select", "while", "until", "do",
		"done", "in", "function", "time", "{", "}",
		"!", "[[", "]]", "coproc",
	};

	for (size_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
		if (!ft_strcmp(keywords[i], arg)) {
			return true;
		}
	}
	return false;
}

void builtin_type(const SimpleCommandP *command, Vars *shell_vars) {
	char buffer[MAX_WORD_LEN] = {0};
	size_t i = 1;

	for (; command->word_list->data[i]; i++) {
		bool isbuiltin = is_builtin(command->word_list->data[i]);
		if (isbuiltin) {
			ft_sprintf(buffer, "%s is a shell builtin\n", command->word_list->data[i]);
			continue;
        }

		bool iskeyword = is_keyword(command->word_list->data[i]);
		if (iskeyword) {
			ft_sprintf(buffer, "%s is a shell keyword\n", command->word_list->data[i]);
			continue;
        }

		char *maybe_bin = hash_find_bin(command->word_list->data[i], shell_vars);
		if (!maybe_bin && !isbuiltin) {
			TYPE_BIN_NOT_FOUND(command->word_list->data[i]);
			continue;
		} else {
			ft_sprintf(buffer, "%s is %s\n", command->word_list->data[i], maybe_bin);
		}
	}

	ft_putstr_fd(buffer, STDOUT_FILENO);
	g_exitno = 0;
}
