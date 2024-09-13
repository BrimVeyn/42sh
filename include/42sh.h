/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   42sh.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:01:25 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/13 17:33:05 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAXISH_H
# define MAXISH_H

extern int g_signal;
extern int g_exitno;
extern int g_debug;
/*_.-=-._.-=-._.-=-._.-=-._.- Includes -._.-=-._.-=-._.-=-._.-=-._.-=-._*/
#include "../libftprintf/header/libft.h"
#include "../include/colors.h"
#include <readline/readline.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include<sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <stdint.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <readline/history.h>

typedef enum {
	GC_GENERAL,
	GC_SUBSHELL,
	GC_ALL,
} type_of_garbage;

#define FREE_POINTERS(...) \
    void *ptrs[] = { __VA_ARGS__ }; \
    for (size_t i = 0; i < sizeof(ptrs)/sizeof(ptrs[0]); i++) { \
        if (ptrs[i] != NULL) { \
            free(ptrs[i]); \
        } \
    } \

//----------------Utils--------------------//
void			free_charchar(char **s);
void			*ft_realloc(void *ptr, size_t oldSize, size_t nbEl, size_t elSize);
int				ft_strlenlen(const char **strstr);
char			**ft_strdupdup(const char **env);
int				there_is_star(char *str);
int				there_is_slash(char *str);
char			*replace_char_greedy(char *str, char c, char by);
char			*gnl(int fd);

typedef enum {
	SIG_PROMPT,
	SIG_HERE_DOC,
	SIG_EXEC,
} type_of_signals;


#ifndef LEXER_H
	#include "../source/lexer/lexer.h"
#endif // !LEXER_H

#ifndef PARSER_H
	#include "../source/parser/parser.h"
#endif // !LEXER_H

#ifndef AST_H
#include "../source/ast/ast.h"
#endif // !AST_H

#ifndef EXEC_H
	#include "../source/exec/exec.h"
#endif // !EXEC_H

#ifndef DEBUG_H
	#include "../source/debug/debug.h"
#endif // !DEBUG_H

#ifndef SIGNAL_H
	#include "../source/signals/signals.h"
#endif // !SIGNAL_H


#ifndef REGEX_H
	#include "../source/regex/regex.h"
#endif // !REGEX_H

//----------------Garbage-------------------//
Garbage			*gc_get(void);
void			gc_init(int n);
void			*gc_add(void *ptr, int n);
void			gc_cleanup(int n);
void			gc_free(void *addr, int n);
void			gc_addcharchar(char **str, int n);
int ft_strstr(char *haystack, char *needle);
int ft_strrstr(char *haystack, char *needle);


#endif
