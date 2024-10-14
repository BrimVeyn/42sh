/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   42sh.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:15:56 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/09 13:13:30 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAXISH_H
# define MAXISH_H

extern int g_signal;
extern int g_exitno;
extern int g_debug;

#include "libft.h"
#include "colors.h"
#include <stdio.h>
#include <sys/cdefs.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#ifndef POSIX_MAX_ID_LEN
# define POSIX_MAX_ID_LEN 255
#endif

#define FREE_POINTERS(...) \
    void *ptrs[] = { __VA_ARGS__ }; \
    for (size_t i = 0; i < sizeof(ptrs)/sizeof(ptrs[0]); i++) { \
        if (ptrs[i] != NULL) { \
            free(ptrs[i]); \
        } \
    } \

#ifndef UTILS_H
	#include "utils.h"
#endif // !UTILS_H

#ifndef LEXER_H
	#include "lexer.h"
#endif // !LEXER_H

#ifndef PARSER_H
	#include "parser.h"
#endif // !LEXER_H

#ifndef AST_H
	#include "ast.h"
#endif // !AST_H

#ifndef EXEC_H
	#include "exec.h"
#endif // !EXEC_H

#ifndef SIGNAL_H
	#include "signals.h"
#endif // !SIGNAL_H

#ifndef REGEX_H
	#include "ft_regex.h"
#endif // !REGEX_H
//
#ifndef ARITHMETIC_H
	#include "../source/parser/arithmetic.h"
#endif // !REGEX_H

#ifndef DEBUG_H
#include "debug.h"
#endif // !DEBUG_H

#endif
