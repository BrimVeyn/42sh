/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_enum.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:04:50 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/15 17:16:28 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_ENUM_H
#define LEXER_ENUM_H

enum type_of_token {
	//---------------
	T_REDIRECTION,
	T_COMMAND,
	T_ARGS, 
	T_EXPANSION, 
	T_PATTERN_MATCHING,
	//---------------
	T_SEPARATOR,
	//---------------
	T_PARSING_ERROR,
};

enum type_of_separator {
	S_AND,//&&
	S_OR, //||
	S_PIPE, //|
	S_SEMI, //;
};

enum type_of_redirection {
	R_INPUT_FILE, //<
	R_OUTPUT_FILE, //>
	R_APPEND_FILE, //>>
	R_HERE_DOC, //<<
	R_AMPERSAND, //<& //>&
};

enum type_of_command {
	C_BUILTIN, //echo, etc...
	C_SHELL, //grep, etc...
};

enum type_of_pattern_matching {
	P_STAR,
	P_QUESTION_MARK,
	P_EXCLAMATION,
};

#endif // !LEXER_ENUM_H
