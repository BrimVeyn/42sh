/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_parameter_expansion.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 14:56:30 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/13 10:58:51 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "parser.h"
#include "libft.h"
#include "utils.h"
#include "exec.h"
#include "ft_regex.h"

char *handle_format(char metachar[3], char *id, char *word, Vars *shell_vars){
	// printf("id: %s\nword: %s\nmetachar: %s\n", id, word, metachar);
	char *value = shell_vars_get_value(shell_vars, id);
	if (!value){
		if (!ft_strcmp(metachar, ":-")){
			return (word);
		}
		else if (!ft_strcmp(metachar, ":=")){
			char *variable = gc(GC_ADD, ft_calloc(ft_strlen(word) + ft_strlen(id) + 3, sizeof(char)), GC_GENERAL);
			ft_sprintf(variable, "%s=%s", id, word);
			string_list_add_or_update(shell_vars->env, variable, GC_ENV);
			return (word);
		}
		else if (!ft_strcmp(metachar, ":?")){
			ft_dprintf(2, " %s\n", word);
			// TODO: ajouter un return FALSE jusquau parser pour prevenir de l'execution des commandes suivantes voir posix
			return "";
			// gc(GC_CLEANUP, GC_ALL);
			// close_all_fds();
			// exit(EXIT_FAILURE);
		}
	}
	if (shell_vars_get_value(shell_vars, id) == NULL)
		return ft_strdup("");
	if (metachar[0] == '#' || metachar[0] == '%'){
		char *value = ft_strdup(shell_vars_get_value(shell_vars, id));
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
		gc(GC_ADD, value, GC_GENERAL);
		return value;
	}
	return ft_strdup(value);
}

char *parser_get_variable_value(char *to_expand, Vars *shell_vars){

	char metachar[3] = {0};
	char *word = NULL;
	char name[POSIX_MAX_ID_LEN] = {0};
	

	if (to_expand[0] == '#'){
		return gc(GC_ADD, ft_itoa(ft_strlen(shell_vars_get_value(shell_vars, to_expand + 1))), GC_GENERAL);
	}

	regex_match_t find_format = regex_match("[:#%]", to_expand);
	int lenght_meta = (regex_match("[:#%][=?#%\\-]", to_expand).re_start != -1) ? 2:1;
	if (find_format.re_start != -1){
		memcpy(name, to_expand, find_format.re_start);
		word = ft_substr(to_expand, find_format.re_end + lenght_meta - 1, ft_strlen(to_expand) - find_format.re_end);
		gc(GC_ADD, word, GC_GENERAL);
		ft_memcpy(metachar, &to_expand[find_format.re_start], sizeof(char) * lenght_meta);
		return handle_format(metachar, name, word, shell_vars);
	}
	
	char *value = shell_vars_get_value(shell_vars, to_expand);
	return value ? value : gc(GC_ADD, ft_strdup(""), GC_SUBSHELL);
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
		regex_match("\\$\\{.*:\\}", el->w_infix).re_start == pos ||
		regex_match("\\$\\{[^\\}]*$", el->w_infix).re_start == pos ||
		(regex_match("\\$\\{[A-Za-z_][A-Za-z0-9_]*[#%:].*\\}", el->w_infix).re_start != pos &&
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

bool parser_parameter_expansion(TokenList *tl, Vars *shell_vars){
	for (uint16_t i = 0; i < tl->size; i++){
		Token *el = tl->data[i];

		if (el->tag == T_WORD){
			regex_match_t result;
			char *value = NULL;
			
			char *home = shell_vars_get_value(shell_vars, "HOME");
			do {
				int index = ft_strstr(el->w_infix, "~");
				if (index == -1)
					break;
				char *start = (index == 0) ? ft_strdup("") : ft_substr(el->w_infix, 0, index - 1);
				char *end = ((size_t)index == ft_strlen(el->w_infix)) ? ft_strdup("") : ft_substr(el->w_infix, index + 1, ft_strlen(el->w_infix) - index);
				char *start_and_home = ft_strjoin(start, home);
				char *new_string = gc(GC_ADD, ft_strjoin(home, end), GC_SUBSHELL);
				gc(GC_FREE, el->w_infix, GC_SUBSHELL);
				el->w_infix = new_string;

				// char value[MAX_WORD_LEN] = {0};
				// ft_memset(value, 0, MAX_WORD_LEN);
				// ft_sprintf(value, "%s%s%s", start, home, end);
				// el->w_infix = gc(GC_ADD, ft_strdup(value), GC_SUBSHELL);
				// printf("VALUE: %s\n", value);
				FREE_POINTERS(start, end, start_and_home);
			} while (1);

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
				else {
					result = regex_match ("\\$\\{[^\\$]*\\}", el->w_infix);
					if (result.re_start != -1){
						value = parser_get_variable_value(gc(GC_ADD, ft_substr(el->w_infix, result.re_start + 2, result.re_end - result.re_start - 3), GC_SUBSHELL), shell_vars);
						// printf("value: %s\n", value);	
					} else
						break;
				}
				// printf("HELLO %s\n", el->w_infix);	
					
				char *re_start = ft_substr(el->w_infix, 0, result.re_start);
				char *re_end = ft_substr(el->w_infix, result.re_end, ft_strlen(el->w_infix));
				char *tmp = ft_strjoin(re_start, value);
				
				el->w_infix = gc(GC_ADD, ft_strjoin(tmp, re_end), GC_GENERAL);
				free(tmp); free(re_start); free(re_end);
				// printf("string: %s\n\n", el->w_infix);

			} while(true);
		}
	}
	return true;
}
