#include "expansion.h"
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

void filename_expansion(StrList * string_list) {
	
	// str_list_print(string_list);

	for (size_t i = 0; i < string_list->size; i++) {
		Str *head = string_list->data[i];
		while (head) {
			if (!head->dquote && !head->squote && is_pattern(head->str, "*?[")) {

                //Open current_directories and read all entries
                //Separate the whole string into smaller parts with FS='/'
				bool keep_dotfiles = true;
				da_create(entries, MatchEntryL, sizeof(MatchEntry), GC_SUBSHELL);

                // for (size_t i = 0; i < entries->size; i++) {
                //     dprintf(2, "full: %s\n", entries->data[i].full_path);
                // }
                // _fatal("caught", 1);

                StringListL *pattern_parts = cut_pattern(head->str);
                // for (size_t i = 0; i < pattern_parts->size; i++) {
                //     dprintf(2, "P[%zu]: %s\n", i, pattern_parts->data[i]);
                //     dprintf(2, "size: %zu\n", ft_strlen(pattern_parts->data[i]));
                // }
                //Iterator over each pattern parts
                int flag;
                for (size_t i = 0; i < pattern_parts->size; i++) {

                    PatternNodeL *pattern_nodes = compile_pattern(pattern_parts->data[i]);
					// print_pattern_nodes(pattern_nodes);

                    if (i == 0) {
                        //TODO: error handling
                        if (!pattern_nodes->size) {
                            get_dir_entries(entries, "/", P_ABSOLUTE_INIT);
                            flag = P_ABSOLUTE;
                            continue;
                        } else {
                            get_dir_entries(entries, ".", P_RELATIVE_INIT);
                            flag = P_RELATIVE;
                        }
                    }

					keep_dotfiles = (pattern_nodes->data[0].type == P_CHAR && pattern_nodes->data[0].c == '.');
					// print_pattern_nodes(pattern_nodes);

                    if ((i > 0 && flag == P_RELATIVE) || (i > 1 && flag == P_ABSOLUTE)) {
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
				// dprintf(2, "KEEP[%zu]: %s\n", i, boolStr(keep_dotfiles));
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
