/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   42sh.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 16:00:12 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/29 13:56:52 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAXISH_H
# define MAXISH_H

/*_.-=-._.-=-._.-=-._.-=-._.- Includes -._.-=-._.-=-._.-=-._.-=-._.-=-._*/

//----------------Garbage-------------------//
void			gc_init(void);
void			*gc_add(void *ptr);
void			gc_cleanup(void);

void free_charchar(char **s);

# define I_READ 0
# define I_WRITE 1

#endif
