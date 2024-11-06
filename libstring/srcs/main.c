/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 17:32:31 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/24 11:38:06 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../include/c_string.h"
void str_info(const string *str);

void str_destroy(string *str){
    free(str->data);
    str->data = NULL;
    str->size = 0;
    str->capacity = 0;
}

void str_assign(string *str, const char *content){
	size_t len = strlen(content);
	str_assign_right_size(str, len);
	memcpy(str->data, content, len + 1);
	str->size = len;
}

//to do: fancy infos
void str_info(const string *str){
    printf("===\ncontent: \"%s\"\nsize: %d\ncapacity: %d\n===\n", str->data, str_length(str), str_capacity(str));
}

string string_init_str(const char *str) {
	string ptr;

	ptr.data = strdup(str);
	ptr.size = strlen(ptr.data);
	ptr.capacity = ptr.size + 1;
	return ptr;
}

string str_strdup(const string *str){
	string new_str;
	// new_str.data = strdup(str->data);
	new_str.size = str->size;
	new_str.capacity = str->capacity;
	new_str.data = calloc(new_str.capacity, sizeof(char));
	memcpy(new_str.data, str->data, new_str.size);
	new_str.data[new_str.size] = '\0';
	return new_str;
}

// int main(int argc, char **argv){
//     (void)argc;
//     (void)argv;
// 	// string str = STRING_L("Hello world!");
//
//  //    /* Testing str_resize && str_set */
//  //    str_resize(&str, 8);
//  //    str_info(&str);
//  //    str_assign(&str, "HELLO WORLD!");
//  //    str_info(&str);
//  //    str_resize(&str, 25);
//  //    str_info(&str);
// 	//
//  //    /* Testing str_reserve */
//  //    str_reserve(&str, 100);
//  //    str_info(&str);
// 	//
//  //    /* Testing str_shrink_to_fit */
// 	// str_shrink_to_fit(&str);
// 	// str_info(&str);
// 	//
//  //    /* Testing str_clear */
// 	// str_clear(&str);
// 	// str_info(&str);
//
//     /* Testing str_append */
// 	// string app = STRING_L("Hello");
// 	// string end = STRING_L(" world!");
// 	// str_append(&app, &end);
// 	// str_info(&app);
// 	// // str_info(&end);
// 	// 
// 	// str_insert(&app, "UUUU", 5);
// 	// str_info(&app);
// 	// str_erase(&app, 5, 4);
// 	// str_info(&app);
// 	// 
// 	// str_pop_front(&app);
// 	// str_info(&app);
// 	// printf("\"o\": %zu\n", str_find(&app, "o"));
// 	// printf("\"o\": %zu\n", str_rfind(&app, "o"));
// 	// string test = str_substr(&app, 0, 4);
// 	// str_info(&test);
// 	//
// 	// // str_destroy(&str);
// 	// str_destroy(&app);
// 	// str_destroy(&end);
// 	
// 	string s1 = STRING_L(LOREM);
// 	string s2 = STRING_L(LOREM2);
// 	#include <time.h>
//     clock_t start, end;
//     double cpu_time_used;
//
//
//     start = clock();
// 	for (int i = 0; i < 10000000; i++){
// 		// volatile int temp = strcmp(s1.data, s2.data);
// 		int temp = str_compare(s1.data, s2.data);
// 		(void)temp;
// 	}
//     end = clock();
//     cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
//     printf("Temps d'exécution strcmp via str_compare : %f secondes\n", cpu_time_used);
//     
// 	start = clock();
// 	for (int i = 0; i < 100000000; i++){
// 		str_compare_simd(&s1, &s2);
// 	}
//     end = clock();
//     cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
//     printf("Temps d'exécution str_compare_simd() : %f secondes\n", cpu_time_used);
// 	// str_info(&s1);
// 	//
// 	start = clock();
// 	for (int i = 0; i < 1000000000; i++){
// 		int temp = strcmp(s1.data, s2.data);
// 		(void)temp;
// 	}
// 	end = clock();
// 	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
// 	printf("Temps d'exécution strcmp : %f secondes\n", cpu_time_used);
//
//     return 0;
// }
