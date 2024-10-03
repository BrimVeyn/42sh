/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hash.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 11:16:37 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/03 17:32:43 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "lexer.h"
#include "libft.h"
#include "parser.h"
#include "utils.h"
#include "ft_regex.h"
#include <unistd.h>
#include <sys/stat.h>


int hash_func(char* s) {
	int n = ft_strlen(s);
    long long p = 31, m = 1e9 + 7;
    long long hash = 0;
    long long p_pow = 1;
    for(int i = 0; i < n; i++) {
        hash = (hash + (s[i] - 'a' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }
	if (hash < 0)
		hash = -hash;
	return hash % TABLE_SIZE;
}

void hash_table_delete_entry(Entry **table, size_t index) {
	Entry *current = table[index];
	while (current) {
		Entry *tmp = current->next;
		gc(GC_FREE, current->command, GC_ENV);
		gc(GC_FREE, current->bin, GC_ENV);
		gc(GC_FREE, current, GC_ENV);
		current = tmp;
	}
	table[index] = NULL;
}

void hash_table_clear(Entry **table) {
	for (int i = 0; i < TABLE_SIZE; i++) {
		if (table[i] != NULL) {
			hash_table_delete_entry(table, i);
		}
	}
}

char *hash_find_bin(char *bin, Vars *shell_vars) {
	StringList *env = shell_vars->env;
	char **path = ft_split(string_list_get_value(env, "PATH"), ':');
	for (int i = 0; path[i]; i++) {
		char *bin_with_path = ft_strjoin(path[i], (char *)gc(GC_ADD, ft_strjoin("/",bin), GC_ENV));

		struct stat file_stat;
		if (stat(bin_with_path, &file_stat) != -1){
			if (file_stat.st_mode & S_IXUSR){
				free_charchar(path);
				return (char *)gc(GC_ADD, bin_with_path, GC_ENV);
			}
		}
		free(bin_with_path);
		bin_with_path = NULL;
	}
	free_charchar(path);
	return NULL;
}

Entry *entry_init(char *command, hash_mode mode) {
	Entry *self = gc(GC_ADD, ft_calloc(1, sizeof(Entry)), GC_ENV);
	self->command = gc(GC_ADD, ft_strdup(command), GC_ENV);
	self->hits = (mode == HASH_ADD_UNUSED) ? 0 : 1;
	self->next = NULL;
	return self;
}

bool hash_table_update(Entry *current, char *entry) {
	if (!ft_strcmp(current->command, entry)) {
		current->hits++;
		return true;
	}
	return false;
}

bool hash_table_add(Entry **table, char *entry, Vars *shell_vars, hash_mode mode) {
	int index = hash_func(entry);

	if (table[index] == NULL) {
		table[index] = entry_init(entry, mode);
		char *maybe_bin = hash_find_bin(table[index]->command, shell_vars);
		if (!maybe_bin) {
			hash_table_delete_entry(table, index);
			return false;
		} else {
			table[index]->bin = maybe_bin;
		}
	} else if (mode != HASH_ADD_UNUSED) {
		Entry *current = table[index];
		if (hash_table_update(current, entry)) 
			return true;
		while (current->next != NULL) {
			if (hash_table_update(current, entry)) 
				return true;
			current = current->next;
		}
		current->next = entry_init(entry, mode);
	}
	return true;
}

void hash_table_print(Entry **table) {
	char buffer[MAX_WORD_LEN] = {0};
	bool hit = false;
	size_t buffer_size = sprintf(buffer, "hits\tcommand\n");
	for (int i = 0; i < TABLE_SIZE; i++) {
		if (table[i] != NULL) {
			Entry *current = table[i];
			while (current) {
				buffer_size += sprintf(buffer + buffer_size, "%4d\t%s\n", current->hits, current->bin);
				hit = true;
				current = current->next;
			}
		}
	}
	if (hit)
		printf("%s", buffer);
}

void hash_table_remove(Entry **table, char *entry) {
	size_t index = hash_func(entry);

	if (table[index] != NULL) {
		hash_table_delete_entry(table, index);
	}
}

char *hash_table_find(Entry **table, char *command) {
	int index = hash_func(command);

	if (table[index] != NULL) {
		return table[index]->bin;
	}
	return NULL;
}

void *hash_interface(hash_mode mode, char *arg, Vars *shell_vars) {
	static Entry *table[TABLE_SIZE] = {0};

	switch(mode) {
		case HASH_ADD_USED:
		case HASH_ADD_UNUSED:
			if (!hash_table_add(table, arg, shell_vars, mode)) {
				HASH_BIN_NOT_FOUND(arg);
				g_exitno = 1;
			}
			break;
		case HASH_FIND:
			return hash_table_find(table, arg);
		case HASH_REMOVE:
			hash_table_remove(table, arg);
			break;
		case HASH_PRINT:
			hash_table_print(table);
			break;
		case HASH_CLEAR:
			hash_table_clear(table);
		default:
			break;
	}
	return NULL;
}

void builtin_hash(const SimpleCommand *command, Vars *shell_vars) {
	size_t i = 1;
	bool delete_mode = false;
	
	if (!command->args[1]) {
		hash_interface(HASH_PRINT, NULL, shell_vars);
		return ;
	}

	if (command->args[i] && regex_match("^-r*$", command->args[i]).is_found) {
		hash_interface(HASH_CLEAR, NULL, shell_vars);
		i += 1;
	}
	for (; command->args[i] && regex_match("^-d*$", command->args[i]).is_found; i++) {
		delete_mode = true;
	}

	if (delete_mode && !command->args[i]) {
		HASH_OPTION_REQUIRES_ARG;
		g_exitno = 1;
		return ;
	}

	for (; command->args[i]; i++) {
		if (delete_mode) {
			hash_interface(HASH_REMOVE, command->args[i], shell_vars);
        } else {
			hash_interface(HASH_ADD_UNUSED, command->args[i], shell_vars);
		}
	}
}
