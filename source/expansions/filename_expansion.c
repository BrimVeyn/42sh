/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filename_expansion.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/27 20:26:39 by bvan-pae          #+#    #+#             */
/*   Updated: 2025/01/02 15:20:13 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expansion.h"
#include "final_parser.h"
#include "libft.h"
#include "utils.h"
#include "dynamic_arrays.h"

#include <stdio.h>
#include <sys/dir.h>

static StringList *cut_pattern(char *const pattern) {

    da_create(list, StringList, sizeof(char *), GC_SUBSHELL);
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

static bool open_first_entries(MatchEntryL *const entries, int *const flag, const size_t pattern_nodes_size) {
	if (!pattern_nodes_size) {
		get_dir_entries(entries, "/", P_ABSOLUTE_INIT);
		(*flag) = P_ABSOLUTE;
		return true;
	} else {
		get_dir_entries(entries, ".", P_RELATIVE_INIT);
		(*flag) = P_RELATIVE;
	}
	return false;
}


static void remove_dotdot(MatchEntryL *const entries) {
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

static void filter_if_match(MatchEntryL *const entries, PatternNodeL *const pattern_nodes) {
	//Iterate over every entries and compute its matching state
	for (size_t i = 0; i < entries->size;) {
		MatchEntry elem = entries->data[i];
		elem.match = match_string(elem.name, pattern_nodes, 0);
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

bool is_dot_or_dotdot(const PatternNodeL *const nodes) {
	return (
		(nodes->size == 1 && nodes->data[0].type == P_CHAR && nodes->data[0].c == '.')
		||
		(nodes->size == 2 && nodes->data[0].type == P_CHAR && nodes->data[0].c == '.'
			&& nodes->data[1].type == P_CHAR && nodes->data[1].c == '.')
	);
}

static bool is_first_dot(const PatternNodeL *const nodes) {
	return (nodes->size && nodes->data[0].type == P_CHAR && nodes->data[0].c == '.');
}

static char *get_pattern(Str *const head, const StrList *const list, size_t i) {
	char *pattern = head->str;
	head->file_exp = true;
	if (head->next != NULL)
		return pattern;
	while (i + 1 < list->size) {
		pattern = gc(GC_ADD, ft_strjoin(pattern, list->data[i + 1]->str), GC_SUBSHELL);
		list->data[i + 1]->file_exp = true;
		if (list->data[i + 1]->next)
			break;
		i++;
	}
	return pattern;
}

static void remove_marked_string(StrList *const list, const size_t i) {
	while (
		(i + 1 < list->size) &&
		(list->data[i + 1]->file_exp == true && list->data[i + 1]->next == NULL)
	) {
		da_delete_index(list, i + 1);
	}
	if (i + 1 < list->size && list->data[i + 1]->file_exp && list->data[i + 1]->next) {
		list->data[i + 1]->str = NULL;
	}
}

static bool skip_pattern(Str **head, const StrList *const string_list, size_t *const i) {
	if ((*head)->next) {
		(*head) = (*head)->next;
	} else {
		(*i)++;
		if ((*i) >= string_list->size)
			return true;
		while ((*i) + 1 < string_list->size && string_list->data[(*i) + 1]->file_exp == true)
			(*i)++;
		(*head) = string_list->data[(*i)];
		if ((*head)->file_exp == true)
			(*head) = (*head)->next;
	}
	return false;
}

void filename_expansion(StrList * string_list) {

	for (size_t i = 0; i < string_list->size; i++) {
		Str *head = string_list->data[i];

		while (head != NULL) {
			char *pattern = get_pattern(head, string_list, i);
			//If we're in quotes or none of the pattern matching chars are found, skip
			if (head->dquote || head->squote || !is_pattern(pattern, "*?[")) {
				if (skip_pattern(&head, string_list, &i))
					break;
				continue;
			}
			//Open current_directories and read all entries
			da_create(entries, MatchEntryL, sizeof(MatchEntry), GC_SUBSHELL);

			//Separate the whole string into smaller parts with IFS='/'
			StringList *pattern_parts = cut_pattern(pattern);
			//This flag is here to tell wheter we're matching from an absolute position or relative to the current dir
			int flag = 0;

			//Depending on bash's implementation it varies wheter these two are the same or not
			bool keep_dot_dotdot;
			bool keep_hidden_files;
			for (size_t i = 0; i < pattern_parts->size; i++) {

				PatternNodeL *pattern_nodes = compile_pattern(pattern_parts->data[i]);
				//Open first dir, wheter '.' or '/'
				if (IS_FIRST_PATTERN && open_first_entries(entries, &flag, pattern_nodes->size))
					continue;

				//We keep dot and dotdot only if there litteraly designated by '.' and '..' respectively
				keep_dot_dotdot = is_first_dot(pattern_nodes);
				keep_hidden_files = is_first_dot(pattern_nodes);

				//Add to entries all matched directories
				if ((i > 0 && flag == P_RELATIVE) || (i > 1 && flag == P_ABSOLUTE))
					open_matched_dirs(&entries);

				//Eventually remove dotfiles
				if (!keep_dot_dotdot)
					remove_dotdot(entries);

				//Compute match for each entries
				filter_if_match(entries, pattern_nodes);
			}
			//If we still have entries, it means we matched with at least one file
			if (entries->size) {
				//If keep_hidden_files, remove hidden files
				remove_hidden_files(entries, keep_hidden_files);
				if (entries->size) {
					sort_entries(entries);
					remove_marked_string(string_list, i);
					join_entries(&head, entries);
					continue;
				}
			} else {
				if (skip_pattern(&head, string_list, &i))
					break;
			}
		}
	}
	return ;
}
