/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parameter_expansion.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 11:16:20 by bvan-pae          #+#    #+#             */
/*   Updated: 2025/01/02 15:57:45 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "libft.h"
#include "utils.h"
#include "ft_regex.h"
#include "expansion.h"

#include <stdio.h>

char *handle_format(char metachar[3], char *id, char *word, Vars *shell_vars){
	// printf("id: %s\nword: %s\nmetachar: %s\n", id, word, metachar);
	// ft_dprintf(2, "WORD: %s\n", word);
	char *value = get_variable_value(shell_vars, id);
	if (!value){
		if (!ft_strcmp(metachar, ":-")){
			return (word);
		}
		else if (!ft_strcmp(metachar, ":=")){
			char *variable = gc(GC_ADD, ft_calloc(ft_strlen(word) + ft_strlen(id) + 3, sizeof(char)), GC_SUBSHELL);
			ft_sprintf(variable, "%s=%s", id, word);
			string_list_add_or_update(shell_vars->env, variable);
			return (word);
		}
		else if (!ft_strcmp(metachar, ":?")){
			ft_dprintf(2, "42sh: %s: %s\n",id, word);
			g_exitno = 1;
			return NULL;
		}
	}
	if (get_variable_value(shell_vars, id) == NULL)
		return ft_strdup("");
	if (metachar[0] == '#' || metachar[0] == '%'){

		char *value = ft_strdup(get_variable_value(shell_vars, id));
		ParameterPatternOpt PatternOpt = 0;

		if (metachar[0] == '#' && metachar[1] == '\0'){ PatternOpt = SMAL_PREFIX; }
		else if (metachar[0] == '#' && metachar[1] == '#' ){ PatternOpt = LONG_PREFIX; }
		else if (metachar[0] == '%' && metachar[1] == '\0'){ PatternOpt = SMAL_SUFFIX; }
		else if (metachar[0] == '%' && metachar[1] == '%' ){ PatternOpt = LONG_SUFFIX; }

		const PatternNodeL *compiled_pattern = compile_pattern(word);
		switch (PatternOpt) {
			case SMAL_PREFIX : {
				int pos = match_string(value, compiled_pattern, SMAL_PREFIX);
				dprintf(2, "pos: %d\n", pos);
				value += pos;
				break;
			}
			case LONG_PREFIX : {
				int pos = match_string(value, compiled_pattern, LONG_PREFIX);
				value += pos;
				dprintf(2, "POS: %d\n", pos);
				break;
			}
			case SMAL_SUFFIX : {
			}
			default: break;
		}

		// gc(GC_ADD, value, GC_SUBSHELL);
		return value;
	}
	return ft_strdup(value);
}

char *parser_get_variable_value(char *to_expand, Vars *shell_vars){

	char metachar[3] = {0};
	char *word = NULL;
	char name[POSIX_MAX_ID_LEN] = {0};
	

	if (to_expand[0] == '#'){
		return gc(GC_ADD, ft_itoa(ft_strlen(get_variable_value(shell_vars, to_expand + 1))), GC_SUBSHELL);
	}
	regex_match_t find_format = regex_match("[:#%]", to_expand);
	int lenght_meta = (regex_match("[:#%][=?#%\\-]", to_expand).re_start != -1) ? 2:1;
	if (find_format.re_start != -1){
		ft_memcpy(name, to_expand, find_format.re_start);
		word = ft_substr(to_expand, find_format.re_end + lenght_meta - 1, ft_strlen(to_expand) - find_format.re_end);
		gc(GC_ADD, word, GC_SUBSHELL);
		ft_memcpy(metachar, &to_expand[find_format.re_start], sizeof(char) * lenght_meta);
		return handle_format(metachar, name, word, shell_vars);
	}
	
	char *value = get_variable_value(shell_vars, to_expand);
	return value ? value : gc(GC_ADD, ft_strdup(""), GC_SUBSHELL);
}

static int get_format_pos(char *full_exp){
	const char metachars[] = ":%#";

	for (int i = 3; full_exp[i]; i++){
		if (i == 2 && full_exp[i] == '#'){
			i++;
		}
		if (ft_strchr(metachars, full_exp[i])){
			return i;
		}
	}
	return -1;
}

static bool is_bad_substitution(char *full_exp){
	int format_pos = get_format_pos(full_exp);
	char *variable_name = NULL;
	const size_t str_len = ft_strlen(full_exp);

	if (format_pos == -1){
		variable_name = ft_substr(full_exp, 2, str_len - 2 - 1);
	} else {
		variable_name = ft_substr(full_exp, 2, format_pos - 2);
	}

	if (
		( !regex_match("^[a-zA-Z_][a-zA-Z0-9_]*$", variable_name).is_found && //XBD NAME
		!regex_match("^#[a-zA-Z_][a-zA-Z0-9_]*$", variable_name).is_found && // #XBD NAME
		!regex_match("^[0-9]+$", variable_name).is_found && //000011---inf
		!regex_match("^#[0-9]*$", variable_name).is_found && //#0011---inf && #
		!regex_match("^\\?$", variable_name).is_found && //?
		!regex_match("^\\@$", variable_name).is_found ) //@
		||
		( regex_match("^\\$\\{.*:\\}$", full_exp).is_found || // !end_with(:)
		  regex_match("^\\$\\{#.*:.*\\}$", full_exp).is_found ) ) // begin_with(#) and has (:)
	{ 
		ft_dprintf(2, "%s: bad substitution\n", full_exp);
		g_exitno = 1;
		free(variable_name);
		return true;
	}
	free(variable_name);
	return false;
}

char *positionals_to_string(Vars * const shell_vars) {
	const StringList *positional = shell_vars->positional;
	char buffer[MAX_WORD_LEN] = {0};

	for (size_t i = 1; i < positional->size; i++) {
		char * const id = ft_ltoa(i);
		const char * const value = string_list_get_value(positional, id);
		free(id);
		if (ft_snprintf(buffer, MAX_WORD_LEN, "%s", value) == -1)
			_fatal("snprintf: buffer overflow", 1);
		if (i + 1 < positional->size) {
			if (ft_snprintf(buffer, MAX_WORD_LEN, " ", positional->data[i]) == -1)
				_fatal("snprintf: buffer overflow", 1);
        }
	}
	return gc(GC_ADD, ft_strdup(buffer), GC_SUBSHELL);
}


static char *find_pattern_or_word(const char *const full_exp) {
	char *match;

	if ((match = ft_strchr(full_exp, '%')) != NULL) {
		if (match[1] == '%') match += 2; 
		else match += 1; 
	} else if ((match = ft_strchr(full_exp, '#')) != NULL) {
		if (match[1] == '#') match += 2;
		else match += 1;
	} else if ((match = ft_strchr(full_exp, ':')) != NULL) {
		if (match[1] == '-' || match[1] == '=' || match[1] == '?' || match[1] == '+')
			match += 2;
		else match = NULL;
	}
	return match;

}


char *parameter_expansion(char * full_exp, Vars *const shell_vars, int *const error){
	regex_match_t result;
	char *value = NULL;
	
	if (is_bad_substitution(full_exp) == true){
		*error = 1; return NULL;
	}

	const char *rhs = find_pattern_or_word(full_exp);
	if (rhs) {
		//TODO: add pattern removal with # ## % %%
		size_t rhs_len = ft_strlen(rhs);
		if (rhs[rhs_len - 1] != '}')
			rhs_len++;
		char * const trimmed_rhs = ft_substr(rhs, 0, rhs_len - 1);
		StringList *expansion_result = do_expansions_word(trimmed_rhs, error, shell_vars, O_PARAMETER);
		if ((*error) != 0) return NULL;

		const char *final_rhs = *expansion_result->data;
		char buffer[MAX_WORD_LEN] = {0};
		const char *final_lhs = gc(GC_ADD, ft_substr(full_exp, 0, rhs - full_exp), GC_SUBSHELL);
		ft_snprintf(buffer, MAX_WORD_LEN, "%s%s}", final_lhs, final_rhs);
		full_exp = gc(GC_ADD, ft_strdup(buffer), GC_SUBSHELL);
	}

	result = regex_match("\\$\\{[0-9]+\\}", full_exp);
	if (result.is_found){
		const size_t start = result.re_start + 2;
		const size_t end = result.re_end - 1;
		char * const tmp = ft_substr(full_exp, start, end - start); //+2 -> skip ${ |  -1 -> skip }
		value = get_variable_value(shell_vars, tmp);
		free(tmp);
		if (!value){
			value = gc(GC_ADD, ft_strdup(""), GC_SUBSHELL);
		}
	}
	else if ((result = regex_match("\\$\\{\\?\\}", full_exp)).is_found){
		value = gc(GC_ADD, ft_itoa(g_exitno), GC_SUBSHELL);
	}
	else if ((result = regex_match("\\$\\{\\@\\}", full_exp)).is_found) {
		value = positionals_to_string(shell_vars);
	}
	else if ((result = regex_match("\\$\\{\\#\\}", full_exp)).is_found) {
		value = gc(GC_ADD, ft_ltoa(shell_vars->positional->size - 1), GC_SUBSHELL);
	}
	else {
		result = regex_match ("\\$\\{[^\\$]*\\}", full_exp);
		if (result.re_start != -1){
			value = parser_get_variable_value(gc(GC_ADD, ft_substr(full_exp, result.re_start + 2, result.re_end - result.re_start - 3), GC_SUBSHELL), shell_vars);
			if (!value){
				*error = 1;
				return NULL;
			}
		} else return NULL;
	}
	char *re_start = ft_substr(full_exp, 0, result.re_start);
	char *re_end = ft_substr(full_exp, result.re_end, ft_strlen(full_exp));
	char *tmp = ft_strjoin(re_start, value);

	full_exp = gc(GC_ADD, ft_strjoin(tmp, re_end), GC_SUBSHELL);
	FREE_POINTERS(tmp, re_start, re_end);

	return ft_strdup(full_exp);
}


