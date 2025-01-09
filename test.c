#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>

int main(void) {
    const char *utf8_str = "Hello 🌍";
    size_t real_len = 0;
    mbstate_t state;
    const char *p = utf8_str;

	setlocale(LC_ALL, "");
    // Initialize the mbstate_t structure
    memset(&state, 0, sizeof(mbstate_t));

    while (*p) {
        wchar_t wc;
        size_t len = mbrtowc(&wc, p, MB_CUR_MAX, &state);

        if (len == (size_t)-1 || len == (size_t)-2) {
            fprintf(stderr, "Invalid multibyte sequence\n");
            break;
        }

        dprintf(2, "Char: %lc, Bytes: %zu\n", wc, len);
        real_len++;
        p += len; // Advance the pointer
    }

    printf("Visible length: %zu\n", real_len);
    return 0;
}

