/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_parameter_expansion.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 09:02:17 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/30 10:57:29 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "libft.h"
#include "utils.h"
#include "exec.h"
#include "regex.h"

char *handle_format(char metachar[3], char *id, char *word, StringList *env){
	// printf("id: %s\nword: %s\nmetachar: %s\n", id, word, metachar);
	char *value = string_list_get_value(env, id);
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
	if (string_list_get_value(env, id) == NULL)
		return ft_strdup("");
	if (metachar[0] == '#' || metachar[0] == '%'){
		char *value = ft_strdup(string_list_get_value(env, id));
		char *regexp = NULL;

		if (metachar[0] == '#'){
			regexp = ft_strjoin("^", word);
		} else {
			regexp = ft_strjoin(word, "$");
		}
		
		match_result result = regex_match(regexp, value);
		if (result.start != -1){
			char *new_value = NULL;
			if (metachar[0] == '#')
				new_value = ft_substr(value, result.end, ft_strlen(value) - result.end);
			else{
				new_value = ft_substr(value, 0, result.start);
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
		return gc_add(ft_ltoa(ft_strlen(to_expand)), GC_SUBSHELL);
	}

	match_result find_format = regex_match("[:#%]", to_expand);
	int lenght_meta = (regex_match("[:#%][=?#%\\-]", to_expand).start != -1) ? 2:1;
	if (find_format.start != -1){
		memcpy(name, to_expand, find_format.start);
		word = ft_substr(to_expand, find_format.end + lenght_meta - 1, ft_strlen(to_expand) - find_format.end);
		//adapt memcpy size
		ft_memcpy(metachar, &to_expand[find_format.start], sizeof(char) * lenght_meta);
		return handle_format(metachar, name, word, env);
	}
	
	char *value = string_list_get_value(env, to_expand);
	return value ? value : gc_add(ft_strdup(""), GC_SUBSHELL);
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
		(regex_match("\\${[^}]*}", before_format).start == pos && regex_match("\\${[#A-Za-z_][A-Za-z0-9_]*[#:%]*.*}", before_format).start != pos &&
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

bool parser_parameter_expansion(TokenList *tl, Vars *shell_vars){
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
					value = gc_add(ft_itoa(g_exitno), GC_SUBSHELL);
				else{

					result = regex_match ("\\${.*}", el->w_infix);
					if (result.start != -1)
						value = parser_get_variable_value(gc_add(ft_substr(el->w_infix, result.start + 2, result.end - result.start - 3), GC_SUBSHELL), shell_vars->env);
					else
						break;
				}
					
				char *start = ft_substr(el->w_infix, 0, result.start);
				char *end = ft_substr(el->w_infix, result.end, ft_strlen(el->w_infix));
				char *tmp = ft_strjoin(start, value);
				
				el->w_infix = gc_add(ft_strjoin(tmp, end), GC_SUBSHELL);
				free(tmp); free(start); free(end);

			} while(regex_match("\\${[A-Za-z_][A-Za-z0-9_]*}", el->w_infix).start != -1);
		}
	}
	return true;
}
