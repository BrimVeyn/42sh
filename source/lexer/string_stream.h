/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_stream.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 14:26:56 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/20 14:34:47 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_STREAM_H
#define STRING_STREAM_H

#include "lexer.h"

typedef struct {
	char *str;
	uint16_t size;
	uint16_t capacity;

} StringStream;

//-----------StringStream utils-------------//

#endif // !STRING_STREAM_H
