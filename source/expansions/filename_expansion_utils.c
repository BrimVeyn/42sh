#include "expansion.h"
#include <sys/dir.h>
#include <dirent.h>

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
					if (node.map[i] == 1)
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
    if (!entries->size || keep_dotfiles) return ;

	for (size_t i = 0; i < entries->size;) {
		if (*(entries->data[i].name) == '.' || !ft_strcmp(entries->data[i].name, ".") || !ft_strcmp(entries->data[i].name, "..")) 
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
    if (entries->size <= 1) return;

    for (size_t i = entries->size; i > 1; i--) {
        bool swapped = false;
        for (size_t j = 0; j < i - 1; j++) {
            if (ft_strcmp(entries->data[j + 1].full_path, entries->data[j].full_path) < 0) {
                MatchEntry tmp = entries->data[j + 1];
                entries->data[j + 1] = entries->data[j];
                entries->data[j] = tmp;
                swapped = true;
            }
        }
        if (!swapped) break; // Exit early if no swaps were made
    }
}

int get_dir_entries(MatchEntryL *list, const char *path, const int flag) {
    DIR *dir = opendir(path);
    if (!dir) {return -1;}

    struct dirent *it;

    while ((it = readdir(dir)) != NULL) {
        char buffer[PATH_MAX] = {0};
        int ret;
        if (flag == P_ABSOLUTE_INIT)
            ret = ft_snprintf(buffer, PATH_MAX, "/%s", it->d_name);
        else
            ret = ft_snprintf(buffer, PATH_MAX, "%s/%s", path, it->d_name);

        if (ret == -1)
            _fatal("snprintf: buffer overflow", 1);

        const char *const full_path = (flag == P_RELATIVE_INIT) ? it->d_name : buffer;

        MatchEntry elem = {
            .name = gc(GC_ADD, ft_strdup(it->d_name), GC_SUBSHELL),
            .type = it->d_type,
            .full_path = gc(GC_ADD, ft_strdup(full_path), GC_SUBSHELL),
        };
        da_push(list, elem);
    }

    closedir(dir);
    return 0;
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
