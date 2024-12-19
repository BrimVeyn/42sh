#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int compare_files(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");
    if (!f1 || !f2) {
        return -1; // Error opening files
    }

    // Get file sizes
    fseek(f1, 0, SEEK_END);
    fseek(f2, 0, SEEK_END);
    long size1 = ftell(f1);
    long size2 = ftell(f2);
    if (size1 != size2) {
        fclose(f1);
        fclose(f2);
        return 0; // Files are different
    }
	if (!size1) {
		fclose(f1);
		fclose(f2);
		return 1;
	}

    // Compare file contents
    fseek(f1, 0, SEEK_SET);
    fseek(f2, 0, SEEK_SET);

    unsigned char *buffer1 = malloc(size1);
    unsigned char *buffer2 = malloc(size2);
    fread(buffer1, 1, size1, f1);
    fread(buffer2, 1, size2, f2);

    int result = memcmp(buffer1, buffer2, size1);

    free(buffer1);
    free(buffer2);
    fclose(f1);
    fclose(f2);

    return result == 0 ? 1 : 0; // 1 if files are equal, 0 if different
}
