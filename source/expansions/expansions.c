/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansions.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/11 11:32:20 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/20 17:29:10 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "libft.h"
#include "signals.h"
#include "utils.h"
#include "ft_readline.h"
#include "final_parser.h"
#include "expansion.h"

#include <fcntl.h>
#include <linux/limits.h>
#include <regex.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void exp_kind_list_print(ExpKindList *list);

char *here_doc(const char *eof, const heredoc_mode mode, Vars * const shell_vars){
	char *input = NULL;
	const char *PS2 = string_list_get_value(shell_vars->set, "PS2");
	static int line_number = 0;

	char filename[] = "/tmp/here_doc_XXXXXX";
	int file_fd = mkstemp(filename);
	if (file_fd == -1) {
		_fatal("mkstemp: failed to create here_doc", 1);
	}
	da_push(g_fdSet, file_fd);

	signal_manager(SIG_HERE_DOC);

	while(++line_number && (input = ft_readline(PS2, shell_vars)) != NULL){
		gc(GC_ADD, input, GC_SUBSHELL);
		if (!ft_strcmp(eof, input) || rl_done == 2) {
			break;
		}
		if (mode == HD_EXPAND){

			da_create(tmp, StringListL, sizeof(char *), GC_SUBSHELL);
			da_push(tmp, input);
			ExpReturn ret = do_expansions(tmp, shell_vars, O_NONE);
			if (ret.error != 0)
				return NULL;

			StringListL *expanded_word = ret.ret;
			ft_dprintf(file_fd, "%s\n", expanded_word->data[0]);
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

void remove_boundaries(Str *exp) {
	static const size_t boundaries[][2] = {
		[EXP_WORD] = {0, 0},
		[EXP_CMDSUB] = {1, 0},
		[EXP_ARITHMETIC] = {3, 2},
		[EXP_VARIABLE] = {0, 0},
	};
	//We aint't trimming for these types
	if (exp->kind == EXP_WORD || exp->kind == EXP_VARIABLE) {
		return ;
	}

	char *word = exp->str;
	size_t len = ft_strlen(word);
	size_t up_bound = boundaries[exp->kind][0];
	size_t low_bound = boundaries[exp->kind][1];

	char *tmp = ft_substr(word, up_bound, len - up_bound - low_bound);
	gc(GC_ADD, tmp, GC_SUBSHELL);
	gc(GC_FREE, word, GC_SUBSHELL);
	exp->str = tmp;
}

void string_list_consume(StrList *str_list, Vars *shell_vars, int *error) {
	for (size_t i = 0; i < str_list->size; i++) {
		char *result = NULL;
		Str *curr = str_list->data[i];
		ExpKind kind = curr->kind;

		if (curr->kind == EXP_WORD || curr->squote) 
			continue;
		else
			remove_boundaries(curr);

		switch (kind) {
			case EXP_CMDSUB: { result = parser_command_substitution(curr->str, shell_vars, error); break;}
			case EXP_ARITHMETIC: {result = parser_arithmetic_expansion(curr->str, shell_vars, error); break;}
			case EXP_VARIABLE: {result = parser_parameter_expansion(curr->str, shell_vars, error); break;}
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

ExpKind identify_exp_begin(char *str) {
	if (!ft_strncmp(str, "$((", 3)) { return EXP_ARITHMETIC; }
	else if (!ft_strncmp(str, "$(", 2)) { return EXP_CMDSUB; }
	else if (!ft_strncmp(str, "${", 2)) { return EXP_VARIABLE; }
	else if (!ft_strncmp(str, "(", 1)) { return EXP_SUB; }
	else { return EXP_WORD; }
}


ExpKind identify_exp_end(const char *str, const ContextMap *context_map, const ExpKind context) {
	if (!ft_strncmp(str, context_map[context].end, ft_strlen(context_map[context].end))) {
		return context;
	} else {
		return EXP_WORD;
	}
}

bool is_var_expand_context(ExpKindList *exp_stack) {
	if (exp_stack->size <= 1) return false;
	for (size_t i = 0; i < exp_stack->size - 1; i++) {
		if (exp_stack->data[i] != EXP_ARITHMETIC) {
			return false;
        }
	}
	return true;
}


void pos_list_print(IntList *list) {
	for (size_t i = 0; i < list->size; i++) {
		ft_dprintf(2, "[%ld]: %d\n", i, list->data[i]);
	}
}

StrList *get_range_list(const char * const candidate, Vars * const shell_vars, const int options, int *const error) {
	
	static const ContextMap map[] = {
		[EXP_ARITHMETIC]	= { .begin = "$((", .end = "))" },
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

	parser_tilde_expansion(cache_stack, word, shell_vars, options);

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

		ExpKind maybe_begin = 0;
		ExpKind maybe_end = 0;

		if (!squote && (maybe_begin = identify_exp_begin(word->data)) != EXP_WORD) {
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
					char * const result = parser_command_substitution(tmp, shell_vars, error);
					if ((*error) != 0) return NULL;

					free(tmp);
					ss_cut(cache_stack, idx);
					ss_push_string(cache_stack, result);
					free(result);
				}
				if (var_expand && top_context == EXP_VARIABLE) {
					da_push(cache_stack, 0);
					const size_t idx = da_peak_back(pos_stack);
					char * const result = parser_parameter_expansion(&cache_stack->data[idx], shell_vars, error);
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


void print_node(Str *node) {
	Str *head = node;
	while (head) {
		printf("n: %s\n", head->str);
		head = head->next;
	}
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

void string_list_split(StrList *list, Vars *shell_vars) {
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


StringListL *string_list_merge(StrList *list) {
	da_create(new, StringListL, sizeof(char *), GC_SUBSHELL);
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

void string_list_push_list(StringListL *lhs, StringListL *rhs) {
	for (size_t i = 0; i < rhs->size; i++) {
		da_push(lhs, rhs->data[i]);
	}
}

void string_erase_nulls(StrList *list) {
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

void printStringList(StringListL *list) {
	for (size_t i = 0; i < list->size; i++) {
		ft_dprintf(2, "[%ld]: %s\n", i, list->data[i]);
	}
}

void quote_removal(const StrList * const list) {

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
					da_push(poses, dpair);
					PosInfo current = { i, j };
					da_push(poses, current);
					dpair = (PosInfo) { -1, -1 }; continue;
				} else { dpair = (PosInfo) { i, j }; }
				continue;
			}
			else if (elem[j] == '\'' && !dquote) {
				squote = !squote;
				if (spair.index != -1) {
					da_push(poses, spair); 
					PosInfo current = { i, j };
					da_push(poses, current);
					spair = (PosInfo) { -1, -1 }; 
					continue;
				} else { spair = (PosInfo) { i, j }; }
				continue;
			}
		}
	}
	for (size_t i = 0; i < poses->size;) {
		da_create(ss, StringStream, sizeof(char), GC_SUBSHELL);
		ss_push_string(ss, list->data[poses->data[i].list_index]->str);

		int offset = 0;
		int origin = poses->data[i].list_index;
		while (i < poses->size && poses->data[i].list_index == origin) {
			da_erase_index(ss, ((size_t) poses->data[i].index - offset));
			offset++;
			i++;
		}

		gc(GC_FREE, list->data[poses->data[i - 1].list_index]->str, GC_SUBSHELL);
		list->data[poses->data[i - 1].list_index]->str = (ss->size == 0) ? NULL : ss_get_owned_slice(ss);
	}
}

char *remove_quotes(char *word) {
	da_create(list, StrList, sizeof(Str *), GC_SUBSHELL);
	Str *tmp = str_init(EXP_WORD, word, false);
	da_push(list, tmp);
	quote_removal(list);
	return list->data[0]->str;
}

bool is_pattern(const char *lhs, const char *rhs) {
	bool dquote = false, squote = false;

	for (size_t i = 0; lhs[i]; i++) {

		if		(lhs[i] == '\"' && !squote) dquote = !dquote;
		else if (lhs[i] == '\'' && !dquote) squote = !squote;

		if (!squote && !dquote && ft_strchr(rhs, lhs[i])) {
			return true;
		}
	}
	return false;
}

typedef struct {
    char *name;
    char *full_path;
    int type;
	bool match;
} MatchEntry;

typedef struct {
    MatchEntry *data;
    size_t size;
    size_t capacity;
    size_t size_of_element;
    int gc_level;
} MatchEntryL;

#include <sys/dir.h>
#include <dirent.h>

int get_dir_entries(MatchEntryL *list, const char *path, const bool join_path) {
    DIR *dir = opendir(path);
    if (!dir) {return -1;}

    struct dirent *it;

    while ((it = readdir(dir)) != NULL) {
        char buffer[PATH_MAX] = {0};
        int ret = ft_snprintf(buffer, PATH_MAX, "%s/%s", path, it->d_name);
        if (ret == -1)
            _fatal("snprintf: buffer overflow", 1);

        MatchEntry elem = {
            .name = gc(GC_ADD, ft_strdup(it->d_name), GC_SUBSHELL),
            .type = it->d_type,
            .full_path = gc(GC_ADD, ft_strdup((join_path) ? buffer : it->d_name), GC_SUBSHELL),
        };
        da_push(list, elem);
    }

    closedir(dir);
    return 0;
}

StringListL *cut_pattern(char *pattern) {

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

typedef enum { P_STAR, P_QMARK, P_RANGE, P_CHAR } PatternType;

typedef struct {
    PatternType type;
    union {
        char *map;
        char c;
    };
} PatternNode;

typedef struct {
    PatternNode *data;
    size_t size;
    size_t capacity;
    size_t size_of_element;
    int gc_level;
} PatternNodeL;


char *compile_range(char **pattern) {
	char *map = gc(GC_CALLOC, 256, sizeof(char), GC_SUBSHELL);

	char *str = *pattern;
	bool reverse = (*str == '^' || *str == '!');
	str += (reverse);
	// dprintf(2, "str: %s\n", str);
	while(*str) {
		if ((*str) == '-') { str++; continue; }
		if ((*str) == ']')
			break;
		if ((*(str + 1)) != 0 && *(str + 1) == '-') {
			unsigned char maybe_start = (*str);
			str++;
			while (*str == '-') str++;
			if (*str) {
				unsigned char maybe_end = (*str);
				unsigned char start = (maybe_start <= maybe_end) ? maybe_start : maybe_end;
				unsigned char end = (maybe_end > maybe_start) ? maybe_end : maybe_start;
				// dprintf(2, "s: %c | e: %c\n", start, end);
				ft_memset(&map[start], 1, end - start + 1);
			}
		} else {
			map[(unsigned char)(*str)] = 1;
		}
		str++;
	}
	*pattern = str;
	for (size_t i = 0; reverse && i < 256; i++)
		map[i] = !map[i];
	return map;
}

PatternNodeL *compile_pattern(char *pattern) {
    da_create(list, PatternNodeL, sizeof(PatternNode), GC_SUBSHELL);

    bool dquote = false, squote = false;
    const char *special = "*?[";

	while(*pattern) {
		if		(*pattern == '\"' && !squote) { dquote = !dquote; pattern++; continue; }
		else if (*pattern == '\'' && !dquote) { squote = !squote; pattern++; continue; }

        if (!ft_strchr(special, *pattern) || squote || dquote) {
            PatternNode node = { .type = P_CHAR, .c = *pattern };
            da_push(list, node);
        } else {
            if (*pattern == '*') {
                PatternNode node = { .type = P_STAR, .c = 0 };
                da_push(list, node);
            } else if (*pattern == '?') {
				PatternNode node = { .type = P_QMARK, .c = 0 };
                da_push(list, node);
            } else { //pattern[i] == [
				if (ft_strchr(pattern, ']')) {
					PatternNode node = { .type = P_RANGE, .c = 0};
					pattern++; //skip [
					// dprintf(2, "pattern: %s\n", pattern);
					node.map = compile_range(&pattern);
					// dprintf(2, "pattern: %s\n", pattern);
					da_push(list, node);
				} else {
					PatternNode node = { .type = P_CHAR, .c = '[' };
					da_push(list, node);
				}
            }
        }
		pattern++;
    }
    return list;
}

typedef struct {
	const char *str;
	size_t size;
} String;

bool match_pattern(int **dp, const String str, const PatternNodeL *pattern, size_t i, size_t j) {
	// dprintf(2, "INIT: i: %zu, j: %zu\n", i, j);
	if (dp[i][j] != -1) {
		// dprintf(2, "RETURN -1: i: %zu, j: %zu\n", i, j);
		return dp[i][j];
    }

	if (i >= str.size && j >= pattern->size) {
		// dprintf(2, "RETURN MATCH: i: %zu, j: %zu\n", i, j);
		return true;
    }
	if (j >= pattern->size) {
		// dprintf(2, "RETURN EOP: i: %zu, j: %zu\n", i, j);
		return false;
	}

	bool match = ( 
		(i < str.size) &&
		((pattern->data[j].type == P_RANGE && pattern->data[j].map[(unsigned int)str.str[i]] == true) ||
		(pattern->data[j].type == P_CHAR && pattern->data[j].c == str.str[i]) ||
        (pattern->data[j].type == P_QMARK))
	);

	if (pattern->data[j].type == P_STAR) {
		// dprintf(2, "STAR: i: %zu, j: %zu\n", i, j);
		bool i_1 = (i < str.size && match_pattern(dp, str, pattern, i + 1, j));
		bool j_1 = (match_pattern(dp, str, pattern, i, j + 1));
		dp[i][j] = (i_1 || j_1);
		return dp[i][j];
	}

	if (match) {
		dp[i][j] = match_pattern(dp, str, pattern, i + 1, j + 1);
		return dp[i][j];
	}

	dp[i][j] = false;
    return dp[i][j];
}

bool match_string(const char *str, const PatternNodeL *pattern_nodes) {

	String string = {
		.str = str,
		.size = ft_strlen(str),
	};

    int **dp = calloc(string.size + 1, sizeof(int *));
    for (size_t i = 0; i < string.size + 1; i++) {
        dp[i] = malloc((pattern_nodes->size + 1) * sizeof(int));
		for (size_t j = 0; j < pattern_nodes->size + 1; j++) {
			dp[i][j] = -1;
		}
    }

	// dprintf(2, "------------------ str: %s ------------\n", str);
    bool match = match_pattern(dp, string, pattern_nodes, 0, 0);
	// dprintf(2, "---------------------------------------\n");

    for(size_t i = 0; i < string.size; i++) {
        free(dp[i]);
    }
    free(dp);

    return match;
}

void print_pattern_nodes(PatternNodeL *nodes) {
    for (size_t i = 0; i < nodes->size; i++) {
        PatternNode node = nodes->data[i];
        dprintf(2, "N[%zu]: ", i);
        switch (node.type) {
            case P_STAR: { dprintf(2, "STAR"); break; }
            case P_QMARK: { dprintf(2, "QMARK"); break; }
			case P_CHAR: { dprintf(2, "C: %c", node.c); break; }
            case P_RANGE: {
				unsigned char buffer[256] = {0};
				size_t len = 0;
				for (size_t i =0; i < 256; i++) { 
					if (node.map[i])
						buffer[len++] = (ft_isprint(i) ? i : 'X');
				}
				buffer[len] = 0;
				dprintf(2, "R: %s", buffer);
			}
            default: break;
        }
        dprintf(2, "\n");
    }
}

void remove_dofiles(MatchEntryL *entries, const bool keep_dotfiles) {
    if (!entries->size) return ;

	for (size_t i = 0; i < entries->size;) {
		if ((*(entries->data[i].name) == '.' && !keep_dotfiles) ||
            !ft_strcmp(entries->data[i].name, ".") || !ft_strcmp(entries->data[i].name, "..")) 
        {
			entries->data[i] = entries->data[entries->size - 1];
			entries->size--;
			continue;
		}
		i++;
	}
}

char *join_entries(const MatchEntryL *entries) {
	da_create(ss, StringStream, sizeof(char), GC_SUBSHELL);

	for (size_t i = 0; i < entries->size; i++) {
		ss_push_string(ss, entries->data[i].full_path);
		if (i + 1 < entries->size)
			da_push(ss, ' ');
	}
	da_push(ss, '\0');
	return ss->data;
}

void sort_entries(MatchEntryL *entries) {
    if (!entries->size) return ;

	for (size_t i = entries->size - 1; i >= 1; i--) {
		for (size_t j = 0; j <= i - 1; j++) {
			if (ft_strcmp(entries->data[j + 1].name, entries->data[j].name) < 0) {
				MatchEntry tmp = entries->data[j + 1];
				entries->data[j + 1] = entries->data[j];
				entries->data[j] = tmp;
			}
		}
	}
}

void filename_expansions(StrList * string_list) {
	
	// str_list_print(string_list);

	for (size_t i = 0; i < string_list->size; i++) {
		Str *head = string_list->data[i];
		while (head) {
			if (!head->dquote && !head->squote && is_pattern(head->str, "*?[")) {

                //Open current_directories and read all entries
                //Separate the whole string into smaller parts with FS='/'
				bool keep_dotfiles;
				da_create(entries, MatchEntryL, sizeof(MatchEntry), GC_SUBSHELL);

                //Use this path variable to keep track of the fall path from origin

                // for (size_t i = 0; i < entries->size; i++) {
                //     dprintf(2, "full: %s\n", entries->data[i].full_path);
                // }
                // _fatal("caught", 1);

                //Set starting entries at current directory.

                StringListL *pattern_parts = cut_pattern(head->str);
                // for (size_t i = 0; i < pattern_parts->size; i++) {
                //     dprintf(2, "P[%zu]: %s\n", i, pattern_parts->data[i]);
                //     dprintf(2, "size: %zu\n", ft_strlen(pattern_parts->data[i]));
                // }
                //Iterator over each pattern parts
                for (size_t i = 0; i < pattern_parts->size; i++) {

                    PatternNodeL *pattern_nodes = compile_pattern(pattern_parts->data[i]);
					// print_pattern_nodes(pattern_nodes);
                    // TODO: check for it every time
					if (i == 0) keep_dotfiles = (pattern_nodes->data[i].type == P_CHAR && pattern_nodes->data[i].c == '.');

                    if (i == 0) {
                        //TODO: error handling
                        // dprintf(2, "size: %zu\n", pattern_nodes->size);
                        //TODO: absolute
                        if (!pattern_nodes->size) {
                            get_dir_entries(entries, "/", false);
                        } else {
                            get_dir_entries(entries, ".", false);
                        }
                    }

                    if (i != 0) {
                        da_create(tmp, MatchEntryL, sizeof(MatchEntry), GC_SUBSHELL);
                        for (size_t j = 0; j < entries->size;) {
                            const MatchEntry elem = entries->data[j];
                            // dprintf(2, "Remaining entry: %s\n", elem.name);
                            if (!(elem.type == DT_DIR)) {
                                entries->data[j] = entries->data[entries->size - 1];
                                entries->size--;
                                continue;
                            } 
                            // dprintf(2, "old_path: %s\n", elem.full_path);
                            get_dir_entries(tmp, elem.full_path, true);       
                            j++;
                        }
                        //Liberate entries
                        entries = tmp;
                    }

					// FIX: add them directly to entries
					// for (size_t i = 0; i < entries->size; i++) {
					// 	dprintf(2, "e: %s\n", entries->data[i].name);
					// }

                    //Iterate over every entries and compute its matching state
					for (size_t i = 0; i < entries->size;) {
						entries->data[i].match = match_string(entries->data[i].name, pattern_nodes);
						if (!entries->data[i].match) {
							//Swap with last element and decremnt size since we don't care about order yet (avoid memmove)
							entries->data[i] = entries->data[entries->size - 1];
							entries->size--;
							continue;
						} 
						i++;
					}
                }
				//if pattern matching found something, replace the original string by the joined entries
				if (entries->size) {
					remove_dofiles(entries, keep_dotfiles);
                    if (entries->size) {
                        sort_entries(entries);
                        head->str = join_entries(entries);
                    }
                }
			}
			head = head->next;
		}
	}
	return ;
}

StringListL *do_expansions_word(char *word, int *error, Vars *const shell_vars, const int options) {
	StrList * const string_list = get_range_list(word, shell_vars, options, error);
	if (*error != 0) return NULL;

	string_list_consume(string_list, shell_vars, error);
	if (*error != 0) return NULL;

	if (options & O_SPLIT)
		string_list_split(string_list, shell_vars);
	string_erase_nulls(string_list);

	filename_expansions(string_list);

	quote_removal(string_list);


	return string_list_merge(string_list);
}

ExpReturn do_expansions(const StringListL * const word_list, Vars * const shell_vars, const int options) {
	ExpReturn ret_value = { .ret = NULL, .error = 0 };

	if (!word_list) return ret_value;

	da_create(arg_list, StringListL, sizeof(char *), GC_SUBSHELL);

	for (size_t it = 0; it < word_list->size; it++) {
		StringListL *const ret = do_expansions_word(word_list->data[it], &ret_value.error, shell_vars, options);
		if (ret_value.error != 0)
			return ret_value;
		string_list_push_list(arg_list, ret);
	}
	ret_value.ret = arg_list;	
	return ret_value;
}
