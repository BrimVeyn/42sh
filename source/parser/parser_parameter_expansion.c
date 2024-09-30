/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_parameter_expansion.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 09:02:17 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/30 14:51:58 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

char *handle_format(char metachar[3], char *id, char *word, StringList *env){
	// printf("id: %s\nword: %s\nmetachar: %s\n", id, word, metachar);
	char *value = string_list_get_value_with_id(env, id);
	if (!value){
		if (!ft_strcmp(metachar, ":-")){
			return (word);
		}
		else if (!ft_strcmp(metachar, ":=")){
			char *variable = gc_add(ft_calloc(ft_strlen(word) + ft_strlen(id) + 3, sizeof(char)), GC_GENERAL);
			ft_sprintf(variable, "%s=%s", id, word);
			string_list_add_or_update(env, variable);
			return (word);
		}
		else if (!ft_strcmp(metachar, ":?")){
			ft_dprintf(2, " %s\n", word);
			gc_cleanup(GC_ALL);
			close_all_fds();
			exit(EXIT_FAILURE);
		}
	}
	if (string_list_get_value_with_id(env, id) == NULL)
		return ft_strdup("");
	if (metachar[0] == '#' || metachar[0] == '%'){
		char *value = ft_strdup(string_list_get_value_with_id(env, id));
		char *regexp = NULL;

		if (metachar[0] == '#'){
			regexp = ft_strjoin("^", word);
		} else {
			regexp = ft_strjoin(word, "$");
		}
		
		regex_match_t result = regex_match(regexp, value);
		if (result.re_start != -1){
			char *new_value = NULL;
			if (metachar[0] == '#')
				new_value = ft_substr(value, result.re_end, ft_strlen(value) - result.re_end);
			else{
				new_value = ft_substr(value, 0, result.re_start);
			}
			free(value);
			value = new_value;
			new_value = NULL;
		}
		free(regexp);
		gc_add(value, GC_GENERAL);
		return value;
	}
	return ft_strdup(value);
}

char *parser_get_variable_value(char *to_expand, StringList *env){

	char metachar[3] = {0};
	char *word = NULL;
	char name[POSIX_MAX_ID_LEN] = {0};
	

	if (to_expand[0] == '#'){
		return gc_add(ft_itoa(ft_strlen(string_list_get_value_with_id(env, to_expand + 1))), GC_GENERAL);
	}

	regex_match_t find_format = regex_match("[:#%]", to_expand);
	int lenght_meta = (regex_match("[:#%][=?#%\\-]", to_expand).re_start != -1) ? 2:1;
	if (find_format.re_start != -1){
		memcpy(name, to_expand, find_format.re_start);
		word = ft_substr(to_expand, find_format.re_end + lenght_meta - 1, ft_strlen(to_expand) - find_format.re_end);
		gc_add(word, GC_GENERAL);
		ft_memcpy(metachar, &to_expand[find_format.re_start], sizeof(char) * lenght_meta);
		return handle_format(metachar, name, word, env);
	}
	
	char *value = string_list_get_value_with_id(env, to_expand);
	return value ? value : gc_add(ft_strdup(""), GC_GENERAL);
}

static int get_format_pos(char *str){
	const char metachars[] = ":%#";
	for (int i = 3; str[i]; i++){
		if (i == 2 && str[i] == '#'){
			i++;
		}
		if (ft_strchr(metachars, str[i])){
			return i;
		}
	}
	return -1;
}

static bool is_parameter_balanced(char *string){
	regex_match_t r1;
	do {
		r1 = regex_match("\\$\\{.*\\}", string);
		string[r1.re_end] = '\0';
		string += r1.re_start + 2;
	} while (r1.re_start != -1);
	return (regex_match("\\$\\{", string).re_start != -1 || regex_match("\\}", string).re_start != -1);
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

	if (regex_match("\\$\\{\\}", before_format).re_start == pos ||
		regex_match("\\$\\{[^\\}]*$", el->w_infix).re_start == pos ||
		(regex_match("\\$\\{[A-Za-z_][A-Za-z0-9_]*[#%:].+\\}", el->w_infix).re_start != pos &&
		regex_match("\\$\\{[\\?#]\\}", el->w_infix).re_start != pos &&
		(regex_match("\\$\\{#?[A-Za-z_][A-Za-z0-9_]*\\}", el->w_infix).re_start != pos &&
		is_parameter_balanced(el->w_infix))))
	{ 
		dprintf(2, "${}: bad substitution\n");
		g_exitno = 1;
		free(before_format);
		return true;
	}
	free(before_format);
	return false;
}

bool parser_parameter_expansion(TokenList *tl, StringList *env){
	for (uint16_t i = 0; i < tl->size; i++){
		Token *el = tl->t[i];

		if (el->tag == T_WORD){
			regex_match_t result;
			char *value = NULL;

			do{
				result = regex_match ("\\$\\{", el->w_infix);
				if (result.re_start == -1){
					break;
				}
				
				if (is_bad_substitution(el, result.re_start) == true){
					return false;
				}

				result = regex_match("\\$\\{\\?\\}", el->w_infix);
				if (result.re_start != -1)
					value = ft_itoa(g_exitno);
				else{
					result = regex_match ("\\$\\{[^\\$]*\\}", el->w_infix);
					if (result.re_start != -1){
						value = parser_get_variable_value(gc_add(ft_substr(el->w_infix, result.re_start + 2, result.re_end - result.re_start - 3), GC_SUBSHELL), env);
					}
					else
						break;
				}
					
				char *re_start = ft_substr(el->w_infix, 0, result.re_start);
				char *re_end = ft_substr(el->w_infix, result.re_end, ft_strlen(el->w_infix));
				char *tmp = ft_strjoin(re_start, value);
				
				el->w_infix = gc_add(ft_strjoin(tmp, re_end), GC_GENERAL);
				free(tmp); free(re_start); free(re_end);
				// printf("string: %s\n\n", el->w_infix);

			} while(true);
		}
	}
	return true;
}
