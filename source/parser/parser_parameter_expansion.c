/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_parameter_expansion.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 14:08:25 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/13 14:09:59 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

char *handle_format(char metachar, char * second_world){
	switch (metachar){
		case '-':
			return (second_world);
		// case '?':
		// 	return ();
		default:
			return ft_strdup("");
	}
}

char *parser_get_env_variable_value(char *name, char **env){
	printf("string %s\n", name);
	for (int i = 0; env[i]; i++){
		char *tmp = ft_strjoin(name, "=");
		if (ft_strncmp(env[i], tmp, ft_strlen(name) + 1) == 0){
			free(tmp);
			return ft_strdup(env[i] + ft_strlen(name) + 1);
		}
		free(tmp);
	}
	return NULL;
}

char *parser_get_variable_value(char *name, char **env){

	char metachar = 0;
	int end_of_name = 0;
	char *second_world = NULL;
	
	//use get_format_pos function bc repetition
	for (int i = 0; name[i]; i++){
		if (name[i] == ':'){
			metachar = name[i + 1];
			second_world = gc_add(ft_substr(name, i + 2, ft_strlen(name) - i), GC_GENERAL);
			end_of_name = i;
			break;
		}
	}
	
	if (metachar){
		ft_memset(name + end_of_name, '\0', ft_strlen(name) - end_of_name);
	}
	
	char *tmp = parser_get_env_variable_value(name, env);
	if (tmp){
		return tmp;
	}
	free(tmp);
	return handle_format(metachar, second_world);
}

static int get_format_pos(char *str){
	for (int i = 3; str[i]; i++){
		if (str[i] == ':'){
			return i + 1;
		}
	}
	return -1;
}

static bool is_bad_substitution(Token *el, int pos){
	int format_pos = get_format_pos(el->w_infix);
	char *before_format = NULL;
	if (format_pos == -1){
		before_format = ft_strdup(el->w_infix);
	} else {
		char *tmp = ft_substr(el->w_infix, 0, format_pos);
		before_format = ft_strjoin(tmp, &el->w_infix[ft_strlen(el->w_infix) - 1]);
		free(tmp);
	}
	// printf("str: %s\n", before_format);

	if (regex_match("\\${}", before_format).start == pos || //match ${} 
		regex_match("\\${[^}]*$", before_format).start == pos || //if missing }
		(regex_match("\\${[^}]*}", before_format).start == pos && regex_match("\\${[A-Za-z_][A-Za-z0-9_:]*}", before_format).start != pos &&
		regex_match("\\${?}", before_format).start != pos))
	{ //check if first char is a number
		dprintf(2, "${}: bad substitution\n");
		g_exitno = 1;
		free(before_format);
		return true;
	}
	free(before_format);
	return false;
}

bool parser_parameter_expansion(TokenList *tl, char **env){
	for (uint16_t i = 0; i < tl->size; i++){
		Token *el = tl->t[i];

		if (el->tag == T_WORD){
			match_result result;
			int pos = -1;
			char *value = NULL;

			do{
				for (int i = 0; el->w_infix[i]; i++){
					if (el->w_infix[i] == '$'){
						pos = i;
						break;
					}
				}
				if (pos == -1){
					break;
				}
				
				if (is_bad_substitution(el, pos) == true){
					return false;
				}

				result = regex_match("\\${?}", el->w_infix);
				if (result.start != -1)
					value = ft_itoa(g_exitno);
				else{

					result = regex_match ("\\${.*}", el->w_infix);
					if (result.start != -1)
						value = parser_get_variable_value(gc_add(ft_substr(el->w_infix, result.start + 2, result.end - result.start - 3), GC_GENERAL), env);
					else
						break;
				}
				
				char *start = ft_substr(el->w_infix, 0, result.start);
				char *end = ft_substr(el->w_infix, result.end, ft_strlen(el->w_infix));
				char *tmp = ft_strjoin(start, value);
				
				el->w_infix = gc_add(ft_strjoin(tmp, end), GC_GENERAL);
				free(tmp); free(start); free(end); free(value);

			} while(regex_match("\\${[A-Za-z_][A-Za-z0-9_]*}", el->w_infix).start != -1);
		}
	}
	return true;
}
