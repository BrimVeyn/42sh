/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tilde_expansion.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 10:52:02 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/10 12:48:16 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "ft_regex.h"
#include "libft.h"
#include "utils.h"
#include "utils.h"
#include <pwd.h>

const char *get_suffix(const char *name, const int len, Vars *shell_vars){
	if (len > 0){
		const struct passwd *passwd = getpwnam(name);
		if (passwd)
			return passwd->pw_dir;
		else
			return NULL;
	} else {
		return get_variable_value(shell_vars, "HOME");
	}
}

void parser_tilde_expansion(StringStream *cache, StringStream *word, Vars *shell_vars, const int options){
	(void)shell_vars;
	if (options & O_ASSIGN){
		regex_match_t result = regex_match("[!-~]+[:/]?", word->data);
		if (!result.is_found){
			return;
		}
		
		//case: ~name				case: :~name							case: =~name
		if (result.re_start == 0 || word->data[result.re_start - 1] == ':' || word->data[result.re_start - 1] == '='){
			const char *name = ft_substr(word->data, result.re_start + 1, result.re_end - 1); // -1 for removing ~
			const char *suffix = get_suffix(name, result.re_end - result.re_start, shell_vars);
			free((void *)name);
			
			da_transfer(cache, word, result.re_start);
			if (suffix){
				for (int i = 0; i < result.re_end - result.re_start; i++){
					ss_pop_front(word);
				}
				ss_push_string(cache, suffix);
			} else {
				da_transfer(cache, word, result.re_end - result.re_start);
			}
		}
	}
	else {
		if (da_peak_front(word) != '~'){
			da_transfer(cache, word, 0);
			return;
		}

		int name_end = 1;
		while(word->data[name_end] && word->data[name_end] != '/')
			name_end++;
		
		const char *name = ft_substr(word->data, 1, name_end - 1); // -1 for removing ~
		const char *suffix = get_suffix(name, name_end - 1, shell_vars);
		free((void *)name);
		if (suffix){
			ss_push_string(cache, suffix);
			for (int i = 0; i < name_end; i++){
				ss_pop_front(word);
			}
			return;
		} else {
			return;
		}
	}
}
