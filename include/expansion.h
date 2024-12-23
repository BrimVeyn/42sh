/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 14:13:02 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/13 11:18:27 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPANSION_H
# define EXPANSION_H

#include "final_parser.h"

#include <stdbool.h>
#include <unistd.h>

typedef enum {
	EXP_ARITHMETIC,
	EXP_VARIABLE,
	EXP_CMDSUB,
	EXP_SUB,
	EXP_WORD,
	EXP_SQUOTE,
	EXP_DQUOTE,
} ExpKind;

typedef struct Str {
	struct Str *next;
	char *str;
	ExpKind kind;
	bool	dquote;
	bool	squote;
} Str;

typedef struct {
	Str		**data;
	size_t	size;
	size_t	capacity;
	size_t	size_of_element;
	int		gc_level;
} StrList;

typedef struct {
	ExpKind *data;
	int gc_level;
	size_t size;
	size_t capacity;
	size_t size_of_element;
} ExpKindList;

typedef struct {
	int list_index;
	int index;
} PosInfo;

typedef struct {
	PosInfo *data;
	size_t size;
	size_t capacity;
	size_t size_of_element;
	int	gc_level;
} PosList;

typedef struct {
	int *data;
	size_t size;
	size_t capacity;
	size_t size_of_element;
	int	gc_level;
} IntList;

typedef struct ContextMap {
	char *begin;
	char *end;
} ContextMap;

//Patterm matching related structs

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

#define P_ABSOLUTE 0
#define P_RELATIVE 1
#define P_ABSOLUTE_INIT 2
#define P_RELATIVE_INIT 4

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

bool is_pattern(const char *lhs, const char *rhs);
int get_dir_entries(MatchEntryL *list, const char *path, const int flag);
void sort_entries(MatchEntryL *entries);
char *join_entries(const MatchEntryL *entries);
void remove_dofiles(MatchEntryL *entries, const bool keep_dotfiles);
void print_pattern_nodes(PatternNodeL *nodes);
bool match_string(const char *str, const PatternNodeL *pattern_nodes);
PatternNodeL *compile_pattern(char *pattern);
void filename_expansion(StrList * string_list);


Str					*str_init(const ExpKind kind, char *str, bool add_to_gc);
void				str_list_print(const StrList *list);
//---------------------------------------------------------------//
char				*parser_parameter_expansion(char * str, Vars *const shell_vars, int * const error);
char				*parser_command_substitution(char *const str, Vars *const shell_vars, int * const error);
char				*parser_arithmetic_expansion(char *const str, Vars *const shell_vars, int *error);
void				parser_tilde_expansion(StringStream *cache, StringStream *word, Vars *shell_vars, const int options);

//-------------------history modules------------//
bool				history_expansion (char **pstring);
void				add_input_to_history(char *input, int *history_fd, Vars *shell_vars);
void				get_history(Vars *shell_vars);

typedef struct {
	StringListL *ret;
	int error;
} ExpReturn;

StringListL *do_expansions_word(char *word, int *error, Vars *const shell_vars, const int options);
ExpReturn do_expansions(const StringListL * const word_list, Vars * const shell_vars, const int options);
char	*remove_quotes(char *word);

char	*expand_prompt_special(const char *ps);


#endif // !EXPANSION
