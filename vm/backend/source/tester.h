/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tester.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 13:24:36 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/19 14:02:08 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TESTER_H
#define TESTER_H

#include <unistd.h>
#include <stdbool.h>

#define SRC_PATH "./tester_src/"
#define INIT_SCRIPT_PATH "./source/init_script.sh"
#define DESTROY_SCRIPT_PATH "./source/destroy_script.sh"

extern char **my_env;

int compare_files(const char *file1, const char *file2);
int execute_unit(char *bin, char *input, char *fileName, int test_number);
int execute_script(char *bin, char *fileName, int test_number);

typedef struct {
	char *fileName;
	char *result;
	int result_len;
} Category;

typedef struct {
	char *partial;
	size_t partial_len;
	bool passed;
} UnitResult;

char *my_basename(const char *path);
UnitResult unit_test(char *fileName, char *line, int test_number);
UnitResult script_test(char *fileName, int test_number);
int compare_files(const char *file1, const char *file2);

#include <dirent.h>

int fill_dir(DIR *src_dir, struct dirent **dir_buffer);
void sort_dir(struct dirent **dir_buffer, int category_count);
void print_dir(struct dirent **dir_buffer);

#endif // !TESTER_H
