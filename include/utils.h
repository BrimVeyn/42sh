/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 10:08:18 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/17 17:08:37 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>

extern int g_exitno;

#define BUFFER_SIZE 1024
#define MAX_WORD_LEN 65536
#define MAX_FILENAME_LEN 1024
#define POSIX_MAX_ID_LEN 255

#define FREE_POINTERS(...) \
    void *ptrs[] = { __VA_ARGS__ }; \
    for (size_t i = 0; i < sizeof(ptrs)/sizeof(ptrs[0]); i++) { \
        if (ptrs[i] != NULL) { \
            free(ptrs[i]); \
        } \
    } \

//----------------Garbage-------------------//

typedef enum {
	GC_GENERAL,
	GC_READLINE,
	GC_ENV,
	GC_SUBSHELL,
	GC_ALL,
} gc_level;

typedef enum {
	GC_ALLOC,
	GC_CALLOC,
	GC_REALLOC,
	GC_ADD,
	GC_MOVE,
	GC_FREE,
	GC_CLEANUP,
	GC_RESET,
	GC_GET,
} gc_mode;

#define GC_LEVELS GC_ALL

typedef struct {
	void **garbage;
	uint32_t size;
	uint32_t capacity;
} Garbage;

void *gc(gc_mode mode, ...);

//------------------------------------------//


//----------------Dynamic arrays--------------------//

typedef struct {
	char	*data;
	size_t	size;
	size_t	capacity;
	size_t	size_of_element;
	int		gc_level;
} StringStream;

#define gc_unique(datatype, garbage_collector_level) \
	gc(GC_CALLOC, 1, sizeof(datatype), garbage_collector_level); \

#define da_create(name, type_of_array, element_size, garbage_collector_level) \
	type_of_array *name = gc(GC_CALLOC, 1, sizeof(type_of_array), garbage_collector_level); \
	(name)->data = gc(GC_CALLOC, 10, element_size, garbage_collector_level); \
	(name)->size = 0; \
	(name)->capacity = 10; \
	(name)->size_of_element = element_size; \
	(name)->gc_level = garbage_collector_level; \

#define da_push(array, new_element) \
	do { \
		if ((array)->size >= (array)->capacity) { \
			(array)->capacity *= 2; \
			(array)->data = gc(GC_REALLOC, (array)->data, (array)->size, (array)->capacity, (array)->size_of_element, (array)->gc_level); \
		} \
		(array)->data[(array)->size++] = new_element; \
	} while (0); \

#define da_push_front(array, new_element) \
	do { \
		if ((array)->size >= (array)->capacity) { \
			(array)->capacity *= 2; \
			(array)->data = gc(GC_REALLOC, (array)->data, (array)->size, (array)->capacity, (array)->size_of_element, (array)->gc_level); \
		} \
		ft_memmove(&(array)->data[1], (array)->data, (array)->size_of_element * (array)->size++); \
		(array)->data[0] = new_element; \
	} while (0); \

#define da_erase_index(array, index) \
	do { \
		if (index < (array)->size)  { \
			ft_memmove(&(array)->data[index], &(array)->data[index + 1], (array)->size_of_element * ((array)->size - index)); \
			(array)->data[--(array)->size] = NULL; \
		} \
	} while (0); \

#define da_clear(array) \
	do { \
		ft_memset((array)->data, 0, (array)->size_of_element * (array)->size); \
		(array)->size = 0; \
	} while (0); \
	
#define da_pop(array) \
	((array)->size == 0 ? 0 : (array)->data[--(array)->size]) \

#define da_peak_back(array) \
	(array)->size == 0 ? 0 : (array)->data[(array)->size - 1] \

#define da_peak_front(array) \
	(array)->size == 0 ? 0 : (array)->data[0] \

#define da_peak_index (array, index) \
	(array)->size > index ? (array)->data[index] : 0 \

#define da_print(array) \
	_Generic((array), \
		StrList *: str_list_print, \
		StringList *: string_list_print, \
		TokenList *: tokenListToString, \
		ExpKindList *: exp_kind_list_print, \
		ExprArray *: expr_array_print \
	)((array))

#define da_pop_front(array) \
	_Generic((array), \
		StringStream *: string_stream_pop_front \
	)(array)

//------------------------------------------//


//----------------Utils--------------------//
void			*ft_realloc(void *ptr, size_t old_size, size_t new_size, size_t element_size);
void			free_charchar(char **array);
size_t			ft_strlenlen(const char **strstr);
char			**ft_strdupdup(const char **env);
char			*replace_char_greedy(char *str, char c, char by);
int				ft_strstr(char *haystack, char *needle);
int				ft_strrstr(char *haystack, char *needle);
int				ft_sprintf(char *buffer, const char *fmt, ...);
int				ft_snprintf(char *buffer, const size_t size_of_buffer, const char *fmt, ...);
void			ft_dprintf(int fd, const char *fmt, ...);
long			ft_atol(const char *str);
char			*ft_ltoa(long n);
char			string_stream_pop_front(StringStream *ss);
//------------------------------------------//

//----------------StringStream------------------//
char			*ss_get_owned_slice(StringStream *ss);
void			ss_push_string(StringStream *ss, char *str);
void			ss_cut(StringStream *ss, size_t new_size);
//------------------------------------------//

//----------------File Utils--------------------//
char			*get_next_line(int fd);
char			*read_whole_file(int fd);
char			*get_line_x(char *in, const size_t n);
//------------------------------------------//

void ss_push_string(StringStream *ss, char *str);
char *ss_get_owned_slice(StringStream *ss);
void ss_cut(StringStream *ss, size_t new_size);

void			fatal(char *msg, int exit_code);
void			error(char *msg, int exit_code);

#endif // !UTILS_H
