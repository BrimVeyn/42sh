/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   42sh.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 16:00:12 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/30 11:44:21 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAXISH_H
# define MAXISH_H 1

/*_.-=-._.-=-._.-=-._.-=-._.- Includes -._.-=-._.-=-._.-=-._.-=-._.-=-._*/

//----------------Garbage-------------------//
void			gc_init(void);
void			*gc_add(void *ptr);
void			gc_cleanup(void);

void free_charchar(char **s);

# define I_READ 0
# define I_WRITE 1

#ifndef LEXER_H
	#include "../source/lexer/lexer.h"
#endif // !LEXER_H

#ifndef PARSER_H
	#include "../source/parser/parser.h"
#endif // !LEXER_H

#ifndef EXEC_H
	#include "../source/exec/exec.h"
#endif // !EXEC_H
//
#ifndef DEBUG_H
	#include "../source/debug/debug.h"
#endif // !DEBUG_H


#endif
