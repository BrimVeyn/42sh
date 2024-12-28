/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filename_expansion.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 20:26:39 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/28 17:43:40 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expansion.h"
#include "utils.h"
#include <sys/dir.h>

static StringListL *cut_pattern(char *pattern) {

    da_create(list, StringListL, sizeof(char *), GC_SUBSHELL);
    char *base = pattern;
    char *match = pattern;

    while ((match = ft_strchr(base, '/')) != NULL) {
        ptrdiff_t size = match - base;
        char *part = gc(GC_ADD, ft_substr(base, 0, size), GC_SUBSHELL);

        da_push(list, part);

        base = match + 1;
    }
    if (base != NULL) {
        char *part = gc(GC_ADD, ft_strdup(base), GC_SUBSHELL);
        da_push(list, part);
    }

    return list;
}

static void open_matched_dirs(MatchEntryL **entries) {
	da_create(tmp, MatchEntryL, sizeof(MatchEntry), GC_SUBSHELL);
	MatchEntryL *old_entries = *entries;

	for (size_t j = 0; j < old_entries->size;) {
		const MatchEntry elem = old_entries->data[j];
		if (!(elem.type == DT_DIR)) {
			old_entries->data[j] = old_entries->data[old_entries->size - 1];
			old_entries->size--;
			continue;
		} 
		get_dir_entries(tmp, elem.full_path, true);
		j++;
	}
	gc(GC_FREE, old_entries->data, GC_SUBSHELL);
	gc(GC_FREE, old_entries, GC_SUBSHELL);
	*(entries) = tmp;
}

#define IS_FIRST_PATTERN (i == 0)

static int open_first_entries(MatchEntryL *const entries, int *const flag, const size_t pattern_nodes_size) {
	if (!pattern_nodes_size) {
		get_dir_entries(entries, "/", P_ABSOLUTE_INIT);
		(*flag) = P_ABSOLUTE;
		return 1;
	} else {
		get_dir_entries(entries, ".", P_RELATIVE_INIT);
		(*flag) = P_RELATIVE;
	}
	return 0;
}

void filename_expansion(StrList * string_list) {

	for (size_t i = 0; i < string_list->size; i++) {
		Str *head = string_list->data[i];
		while (head) {
			if (head->dquote || head->squote || !is_pattern(head->str, "*?[")) {
				head = head->next;
				continue;
			}
			//Open current_directories and read all entries
			//Separate the whole string into smaller parts with FS='/'
			bool keep_dotfiles = true;
			da_create(entries, MatchEntryL, sizeof(MatchEntry), GC_SUBSHELL);

			StringListL *pattern_parts = cut_pattern(head->str);
			int flag = 0;

			for (size_t i = 0; i < pattern_parts->size; i++) {

				PatternNodeL *pattern_nodes = compile_pattern(pattern_parts->data[i]);
				if (IS_FIRST_PATTERN) {
					if (open_first_entries(entries, &flag, pattern_nodes->size))
						continue;
				}

				keep_dotfiles = (pattern_nodes->data[0].type == P_CHAR && pattern_nodes->data[0].c == '.');

				if ((i > 0 && flag == P_RELATIVE) || (i > 1 && flag == P_ABSOLUTE))
					open_matched_dirs(&entries);

				if (!keep_dotfiles) {
					for (size_t j = 0; j < entries->size;) {
						MatchEntry elem = entries->data[j];
						if (!ft_strcmp(elem.name, ".") || !ft_strcmp(elem.name, "..")) {
							entries->data[j] = entries->data[entries->size - 1];
							entries->size--;
							continue;
						}
						j++;
					}
				}

				//Iterate over every entries and compute its matching state
				for (size_t i = 0; i < entries->size;) {
					MatchEntry elem = entries->data[i];
					elem.match = match_string(elem.name, pattern_nodes);
					//If the entry didn't match the pattern, remove it.
					if (!elem.match) {
						//Swap with last element and decremnt size since we don't care about order yet (avoid memmove)
						entries->data[i] = entries->data[entries->size - 1];
						entries->size--;
						continue;
					} 
					i++;
				}
			}
			//If we still have entries, it means we matched with at least one file
			if (entries->size) {
				remove_dofiles(entries, keep_dotfiles);
				if (entries->size) {
					sort_entries(entries);
					join_entries(&head, entries);
					//FIX: why continue doesn't work
					break;
				}
			}
			head = head->next;
		}
	}
	return ;
}
