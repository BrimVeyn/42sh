/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tilde_expansion.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 15:41:40 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/27 10:27:16 by bvan-pae         ###   ########.fr       */
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
		char *maybe_home = get_variable_value(shell_vars, "HOME");
		if (!maybe_home){
			maybe_home = getenv("HOME");
		}
		return maybe_home;
	}
}

void tilde_expansion(StringStream *cache, StringStream *word, Vars *shell_vars, const int options){
	(void)shell_vars;


	do {
		regex_match_t result = {0};

		if (!ss_to_string(word))
			return;

		if (options & O_ASSIGN){
			result = regex_match("~[!-.0-9;-~]*", ss_to_string(word));
		} else {
			result = regex_match("^~[!-.0-~]*", ss_to_string(word));
		}

		if (!result.is_found)
			return;

		size_t word_end = result.re_end;
		const size_t word_start = result.re_start;

		const size_t word_len = word_end - word_start;

		bool is_at_start = (result.re_start == 0);
		bool is_after_delimiter = (options & O_ASSIGN && (word->data[result.re_start - 1] == ':' || word->data[result.re_start - 1] == '='));

		if (is_at_start || is_after_delimiter){
			//-1 for removing ~
			const char *name = ft_substr(ss_to_string(word), word_start + 1, word_len - 1);
			const char *suffix = get_suffix(name, word_len - 1, shell_vars);
			free((void *)name);

			da_transfer(cache, word, result.re_start);

			if (suffix){
				//removing ~name
				for (size_t i = 0; i < word_len; i++){
					ss_pop_front(word);
				}
				ss_push_string(cache, suffix);
			} else {
				//not changing ~name
				da_transfer(cache, word, word_len);
			}
		} else {
			return;
		}
	} while (options & O_ASSIGN);
}
