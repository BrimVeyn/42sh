/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   42sh.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:01:25 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/05 15:21:37 by nbardavi         ###   ########.fr       */
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

//----------------Garbage-------------------//
void			gc_init(void);
void			*gc_add(void *ptr);
void			gc_cleanup(void);
void			gc_free(void *addr);
void			gc_addcharchar(char **str);

//----------------Utils--------------------//
void			free_charchar(char **s);
void			*ft_realloc(void *ptr, size_t oldSize, size_t nbEl, size_t elSize);
int				ft_strlenlen(char **strstr);
char			**ft_strdupdup(char **env);
int				there_is_star(char *str);
int				there_is_slash(char *str);

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

#endif
