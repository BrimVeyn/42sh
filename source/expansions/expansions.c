/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansions.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 11:32:20 by bvan-pae          #+#    #+#             */
/*   Updated: 2025/01/03 13:55:41 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "libft.h"
#include "signals.h"
#include "utils.h"
#include "ft_readline.h"
#include "final_parser.h"
#include "expansion.h"
#include "dynamic_arrays.h"

#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/dir.h>
#include <dirent.h>

void exp_kind_list_print(ExpKindList *list);

char *here_doc(const char *eof, const heredoc_mode mode, Vars * const shell_vars){
	char *input = NULL;
	char *PS2 = string_list_get_value(shell_vars->set, "PS2");
	if (PS2) { PS2 = prompt_expansion(PS2, shell_vars); }

	static int line_number = 0;

	char filename[] = "/tmp/here_doc_XXXXXX";
	const int file_fd = mkstemp(filename);
	if (file_fd == -1) {
		_fatal("mkstemp: failed to create here_doc", 1);
	}
	da_push(g_fdSet, fd_init(file_fd, FD_CHILD));

	signal_manager(SIG_HERE_DOC);

	while(++line_number && (input = ft_readline(PS2, shell_vars)) != NULL){
		gc(GC_ADD, input, GC_SUBSHELL);
		if (!ft_strcmp(eof, input) || rl_done == 2) {
			break;
		}
		if (mode == HD_EXPAND){
			da_create(tmp, StringList, sizeof(char *), GC_SUBSHELL);
			da_push(tmp, input);
			const ExpReturn ret = do_expansions(tmp, shell_vars, O_NONE);
			if (ret.error != 0)
				return NULL;

			const StringList *const expanded_word = ret.ret;
			ft_dprintf(file_fd, "%s\n", expanded_word->data[0]);
			gc(GC_FREE, ret.ret->data, GC_SUBSHELL);
			gc(GC_FREE, ret.ret, GC_SUBSHELL);
			gc(GC_FREE, tmp->data, GC_SUBSHELL);
			gc(GC_FREE, tmp, GC_SUBSHELL);

		} else if (mode == HD_NO_EXPAND){
			ft_dprintf(file_fd, "%s\n", input);
		}
	}

	signal_manager(SIG_PROMPT);
	if (!input){
		ft_dprintf(2, "42sh: warning: here-document at line %d delimited by end-of-file (wanted `%s')\n", line_number, eof);
	}
	else if (rl_done == 2){
		rl_done = 0;
		return NULL;
	}

	remove_fd_set(file_fd);
	close(file_fd);
	return gc(GC_ADD, ft_strdup(filename), GC_SUBSHELL);
}

static void remove_boundaries(Str *exp) {
	static const size_t boundaries[][2] = {
		[EXP_WORD] = {0, 0},
		[EXP_CMDSUB] = {1, 0},
		[EXP_ARITHMETIC] = {3, 2},
		[EXP_VARIABLE] = {0, 0},
		[EXP_PROC_SUB_OUT] = {2, 1},
		[EXP_PROC_SUB_IN] = {2, 1},
	};
	//We aint't trimming for these types
	if (exp->kind == EXP_WORD || exp->kind == EXP_VARIABLE) {
		return ;
	}

	const char *const word = exp->str;
	const size_t len = ft_strlen(word);
	const size_t up_bound = boundaries[exp->kind][0];
	const size_t low_bound = boundaries[exp->kind][1];

	char *const tmp = ft_substr(word, up_bound, len - up_bound - low_bound);
	gc(GC_ADD, tmp, GC_SUBSHELL);
	gc(GC_FREE, word, GC_SUBSHELL);
	exp->str = tmp;
}

static void string_list_consume(StrList *str_list, Vars *shell_vars, int *error) {
	for (size_t i = 0; i < str_list->size; i++) {
		const char *result = NULL;
		Str *const curr = str_list->data[i];
		const ExpKind kind = curr->kind;

		if (curr->kind == EXP_WORD || curr->squote) 
			continue;
		else
			remove_boundaries(curr);

		switch (kind) {
			case EXP_CMDSUB: { result = command_substitution(curr->str, shell_vars, error); break;}
			case EXP_ARITHMETIC: {result = arithmetic_expansion(curr->str, shell_vars, error); break;}
			case EXP_VARIABLE: {result = parameter_expansion(curr->str, shell_vars, error); break;}
			case EXP_PROC_SUB_IN: {result = process_substitution(curr->str, curr->kind, shell_vars, error); break;}
			case EXP_PROC_SUB_OUT: {result = process_substitution(curr->str, curr->kind, shell_vars, error); break;}
			default: {}
		}
		
		if (*error == 1)
			return ;

		if (result) {
			gc(GC_FREE, str_list->data[i]->str, GC_SUBSHELL);
			str_list->data[i]->str = gc(GC_ADD, result, GC_SUBSHELL);
		} else {
			str_list->data[i]->str = NULL;
		}
	}
}

static ExpKind identify_exp_begin(char *str, const ExpKind context) {
	if (!ft_strncmp(str, "$((", 3)) { return EXP_ARITHMETIC; }
	else if (!ft_strncmp(str, "<(", 2)) { return EXP_PROC_SUB_IN; }
	else if (!ft_strncmp(str, ">(", 2)) { return EXP_PROC_SUB_OUT; }
	else if (!ft_strncmp(str, "$(", 2)) { return EXP_CMDSUB; }
	else if (!ft_strncmp(str, "${", 2)) { return EXP_VARIABLE; }
	else if (*str == '(' && (context == EXP_ARITHMETIC || context == EXP_CMDSUB)) { return EXP_SUB; }
	else { return EXP_WORD; }
}


static ExpKind identify_exp_end(const char *str, const ContextMap *context_map, const ExpKind context) {
	if (!ft_strncmp(str, context_map[context].end, ft_strlen(context_map[context].end))) {
		return context;
	} else {
		return EXP_WORD;
	}
}

static bool is_var_expand_context(ExpKindList *exp_stack) {
	if (exp_stack->size <= 1) return false;
	for (size_t i = 0; i < exp_stack->size - 1; i++) {
		if (exp_stack->data[i] != EXP_ARITHMETIC) {
			return false;
        }
	}
	return true;
}

static StrList *get_range_list(const char * const candidate, Vars * const shell_vars, const int options, int *const error) {
	
	static const ContextMap map[] = {
		[EXP_ARITHMETIC]	= { .begin = "$((", .end = "))" },
		[EXP_PROC_SUB_IN]	= { .begin = "<(",	.end = ")" },
		[EXP_PROC_SUB_OUT]	= { .begin = ">(",	.end = ")" },
		[EXP_CMDSUB]		= { .begin = "$(",	.end = ")"},
		[EXP_VARIABLE]		= { .begin = "${",	.end = "}" },
		[EXP_SUB]			= { .begin = "(",	.end = ")" },
	};

	da_create(self, StrList, sizeof(Str *), GC_SUBSHELL);

	da_create(exp_stack, ExpKindList, sizeof(ExpKind), GC_SUBSHELL);
	da_create(pos_stack, IntList, sizeof(int), GC_SUBSHELL);

	da_create(word, StringStream, sizeof(char), GC_SUBSHELL);
	da_create(cache_stack, StringStream, sizeof(char), GC_SUBSHELL);
	ss_push_string(word, candidate);

	tilde_expansion(cache_stack, word, shell_vars, options);

	bool squote = false, dquote = false, can_push = (word->size == 0);

	while (word->size) {
		const ExpKind top_context = da_peak_back(exp_stack);
		const ExpKind bottom_context = da_peak_front(exp_stack);
		const bool	var_expand = is_var_expand_context(exp_stack);
		const char current_char = da_peak_front(word);

		if (exp_stack->size == 0) {
			if		(current_char == '\"' && !squote) dquote = !dquote;
			else if (current_char == '\'' && !dquote) squote = !squote;
		}

		while (word->size >= 2 && *word->data == '\\') {
			if ((word->data[1] == '$' || word->data[1] == '`' || 
				word->data[1] == '\\' || word->data[1] == '\n') && !squote)
			{
				da_push(cache_stack, da_pop_front(word));
				da_push(cache_stack, da_pop_front(word));
				continue;
			} else break;
		}

		ExpKind maybe_begin = 0;
		ExpKind maybe_end = 0;

		if (!squote && (maybe_begin = identify_exp_begin(word->data, top_context)) != EXP_WORD) {
			if (!exp_stack->size && can_push) {
				Str * const res = str_init(EXP_WORD, ss_get_owned_slice(cache_stack), false);
				da_push(self, res);
			}
			da_push(exp_stack, maybe_begin);
			da_push(pos_stack, cache_stack->size);
			for (size_t i = 0; i < ft_strlen(map[maybe_begin].begin); i++) {
				da_push(cache_stack, da_pop_front(word));
			}
			continue;
		}
		if (!squote && exp_stack->size && (maybe_end = identify_exp_end(word->data, map, top_context)) != EXP_WORD) {
			if (maybe_end == top_context) {
				da_pop(exp_stack);
				for (size_t i = 0; i < ft_strlen(map[maybe_end].end); i++) {
					da_push(cache_stack, da_pop_front(word));
				}
				if (bottom_context == EXP_ARITHMETIC && top_context == EXP_CMDSUB) {
					da_push(cache_stack, 0);
					const size_t idx = da_peak_back(pos_stack);
					char * const tmp = ft_substr(&cache_stack->data[idx], 1, ft_strlen(&cache_stack->data[idx]) - 1);
					char * const result = command_substitution(tmp, shell_vars, error);
					if ((*error) != 0) return NULL;

					free(tmp);
					ss_cut(cache_stack, idx);
					ss_push_string(cache_stack, result);
					free(result);
				}
				if (var_expand && top_context == EXP_VARIABLE) {
					da_push(cache_stack, 0);
					const size_t idx = da_peak_back(pos_stack);
					char * const result = parameter_expansion(&cache_stack->data[idx], shell_vars, error);
					if ((*error) != 0) return NULL;

					if (result) {
						ss_cut(cache_stack, idx);
						ss_push_string(cache_stack, result);
						free(result);
					} else {
						ss_cut(cache_stack, idx);
						ss_push_string(cache_stack, "");
					}
				}
				if (!exp_stack->size) {
					Str * const res = str_init(maybe_end, ss_get_owned_slice(cache_stack), false);
					res->squote = squote; res->dquote = dquote;
					da_push(self, res);
				}
				da_pop(pos_stack);
			} 
			can_push = false;
		} else {
			da_push(cache_stack, da_pop_front(word));
			can_push = true;
		}
	}
	if (can_push) {
		Str * const res = str_init(EXP_WORD, ss_get_owned_slice(cache_stack), false);
		da_push(self, res);
	}
	gc(GC_FREE, cache_stack->data, GC_SUBSHELL);
	gc(GC_FREE, word->data, GC_SUBSHELL);
	return self;
}


void str_add_back(Str **lst, Str *new_value) {
	Str	*temp;

	if (*lst == NULL) {
		*lst = new_value;
	} else {
		temp = *lst;
		while (temp->next)
			temp = temp->next;
		temp->next = new_value;
	}
}

static void string_list_split(StrList *list, Vars *shell_vars) {
	char *IFS = string_list_get_value(shell_vars->set, "IFS");
	if (!IFS) { IFS = "\n\t "; }

	unsigned char map[32] = {[0 ... 31] = 0};

	for (size_t i = 0; IFS[i]; i++) {
		const unsigned char c = IFS[i];
		map[c / 8] |= (1 << (c % 8));
	}

	for (size_t i = 0; i < list->size; i++) {
		if (list->data[i]->kind == EXP_WORD || list->data[i]->squote || list->data[i]->dquote)
			continue;
		Str *node = NULL;
		char *curr_str = list->data[i]->str;
		const size_t str_len = ft_strlen(curr_str);
		for (size_t it = 0; it < str_len; it++) {

			const size_t start = it;
			while (it < str_len && !(map[curr_str[it] / 8] & (1 << (curr_str[it] % 8)))) {
				it++;
			}
			const size_t end = it;
			const char match_char = curr_str[it];

			if (start == end && !(match_char == ' ' || match_char == '\n' || match_char == '\t')) {
				str_add_back(&node, str_init(list->data[i]->kind, ft_strdup(""), true));
			} else if (start != end) {
				str_add_back(&node, str_init(list->data[i]->kind, ft_substr(curr_str, start, end - start), true));
			}
		}
		gc(GC_FREE, list->data[i]->str, GC_SUBSHELL);
		gc(GC_FREE, list->data[i], GC_SUBSHELL);
		list->data[i] = node;
	}
}


static StringList *string_list_merge(StrList *list) {
	da_create(new, StringList, sizeof(char *), GC_SUBSHELL);
	da_create(ss, StringStream, sizeof(char), GC_SUBSHELL);
	for (size_t i = 0; i < list->size; i++) {
		Str *curr = list->data[i];
		Str *next = (i < list->size - 1) ? list->data[i + 1] : NULL;
		if (curr->kind == EXP_WORD) {
			ss_push_string(ss, curr->str);
			if (!next || next->kind == EXP_WORD) {
				da_push(new, ss_get_owned_slice(ss));
			}
		} else {
			Str *node = curr;
			while (node) {
				ss_push_string(ss, node->str);
				if (!(node->next == NULL && next)) {
					da_push(new, ss_get_owned_slice(ss));
				}
				node = node->next;
			}
		}
	}
	return new;
}

static void string_list_push_list(StringList *lhs, StringList *rhs) {
	for (size_t i = 0; i < rhs->size; i++) {
		da_push(lhs, rhs->data[i]);
	}
}

static void string_erase_nulls(StrList *list) {
	for (size_t i = 0; i < list->size;) {
		Str *curr = list->data[i];
		if (!curr || !curr->str) {
			da_erase_index(list, i);
			if (list->data[i]) 
				list->data[i]->kind = EXP_SUB;
			continue;
		} else {
			i++;
		}
	}
}

static void pos_insert_sorted(PosList *list, PosInfo elem) {
	if (!list->size) {
		da_push(list, elem); 
		return;
    }

	size_t i;
	for (i = 0; i < list->size && elem.list_index > list->data[i].list_index; i++);
	// dprintf(2, "i: %zu\n", i);
	for (; i < list->size && elem.index > list->data[i].index; i++);
	// dprintf(2, "ia: %zu\n", i);

	// dprintf(2, "candidate: %d %d, insert at: %zu\n", elem.list_index, elem.index, i);
	da_insert(list, elem, i);
	// dprintf(2, "-------\n");
	// for (size_t i = 0; i < list->size; i++) {
	// 	dprintf(2, "str: %d, %d\n", list->data[i].list_index, list->data[i].index);
	// }
	// dprintf(2, "-------\n");
	
}

static void quote_removal(const StrList * const list) {

	PosInfo spair = { -1, -1 };
	PosInfo dpair = { -1, -1 };
	bool squote = false, dquote = false;

	da_create(poses, PosList, sizeof(PosInfo), GC_SUBSHELL);

	for (size_t i = 0; i < list->size; i++) {
		const char *elem = list->data[i]->str;
		if (list->data[i]->kind != EXP_WORD)
			continue;

		for (size_t j = 0; elem[j]; j++) {
			if	(elem[j] == '\"' && !squote) {
				dquote = !dquote;
				if (dpair.index != -1) {
					pos_insert_sorted(poses, dpair);
					PosInfo current = { i, j };
					pos_insert_sorted(poses, current);
					dpair = (PosInfo) { -1, -1 }; continue;
				} else { dpair = (PosInfo) { i, j }; }
				continue;
			}
			else if (elem[j] == '\'' && !dquote) {
				squote = !squote;
				if (spair.index != -1) {
					pos_insert_sorted(poses, spair); 
					PosInfo current = { i, j };
					pos_insert_sorted(poses, current);
					spair = (PosInfo) { -1, -1 }; 
					continue;
				} else { spair = (PosInfo) { i, j }; }
				continue;
			} else if (elem[j] == '\\' && !squote) {
				if (dquote && elem[j + 1] &&
					!(elem[j + 1] == '$' || elem[j + 1] == '`' || elem[j + 1] == '\"' ||
					 elem[j + 1] == '\n' || elem[j + 1] == '\\')) {
					continue;
				}
				PosInfo current = { i, j };
				pos_insert_sorted(poses, current);
				j++; //skip '\'
				continue; //skip the escaped character
			}
		}
	}

	// for (size_t i = 0; i < poses->size; i++) {
	// 	dprintf(2, "str: %s, %d\n", list->data[poses->data[i].list_index]->str, poses->data[i].index);
	// }

	for (size_t i = 0; i < poses->size;) {
		da_create(ss, StringStream, sizeof(char), GC_SUBSHELL);

		// dprintf(2, "i = %zu\n", i);
		//Retain current index aswell as string
		const int list_index = poses->data[i].list_index;
		const char *const input = list->data[list_index]->str;

		for (int j = 0; input[j]; j++) {
			//if i == j we don't copy it
			if (poses->data[i].list_index == list_index && poses->data[i].index == j) {
				i++;
				continue;
            }
			da_push(ss, input[j]);
		}
		// dprintf(2, "ss: %s\n", ss->data);
		//replace the string with the one without quotes and '\'
		gc(GC_FREE, list->data[list_index]->str, GC_SUBSHELL);
		list->data[list_index]->str = (ss->size == 0) ? NULL : ss_get_owned_slice(ss);
	}
}

char *remove_quotes(char *word) {
	da_create(list, StrList, sizeof(Str *), GC_SUBSHELL);
	Str *tmp = str_init(EXP_WORD, word, false);
	da_push(list, tmp);
	quote_removal(list);
	return list->data[0]->str;
}

StringList *do_expansions_word(char *word, int *error, Vars *const shell_vars, const int options) {
	StrList * const str_list = get_range_list(word, shell_vars, options, error);

	/*str_list_print(str_list);*/
	if (*error != 0) return NULL;

	string_list_consume(str_list, shell_vars, error);
	/*str_list_print(str_list);*/
	if (*error != 0) return NULL;

	if (options & O_SPLIT)
		string_list_split(str_list, shell_vars);

	string_erase_nulls(str_list);

	if (options & O_PATTERN)
		return string_list_merge(str_list);

	// str_list_print(str_list);
	if (!(options & O_PARAMETER))
		filename_expansion(str_list);

	quote_removal(str_list);

	return string_list_merge(str_list);
}

ExpReturn do_expansions(const StringList * const word_list, Vars * const shell_vars, const int options) {
	ExpReturn ret_struct = { 
		.ret = NULL,
		.error = 0 
	};

	if (!word_list) return ret_struct;
	da_create(ret_list, StringList, sizeof(char *), GC_SUBSHELL);

	for (size_t it = 0; it < word_list->size; it++) {
		StringList *const ret = do_expansions_word(word_list->data[it], &ret_struct.error, shell_vars, options);
		if (ret_struct.error != 0)
			return ret_struct;

		string_list_push_list(ret_list, ret);
	}
	ret_struct.ret = ret_list;	
	return ret_struct;
}
