/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   42sh.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 16:00:12 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/02 10:06:22 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAXISH_H
# define MAXISH_H

extern int g_signal;
/*_.-=-._.-=-._.-=-._.-=-._.- Includes -._.-=-._.-=-._.-=-._.-=-._.-=-._*/
#include "../libftprintf/header/libft.h"
#include "../include/colors.h"
#include <readline/readline.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#include <stdint.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <readline/history.h>

//----------------Garbage-------------------//
void			gc_init(void);
void			*gc_add(void *ptr);
void			gc_cleanup(void);
void gc_free(void *addr);

void			free_charchar(char **s);

# define I_READ 0
# define I_WRITE 1

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

#ifndef EXEC_H
	#include "../source/exec/exec.h"
#endif // !EXEC_H

#ifndef DEBUG_H
	#include "../source/debug/debug.h"
#endif // !DEBUG_H

#ifndef SIGNAL_H
	#include "../source/signals/signals.h"
#endif // !SIGNAL_H

#endif
