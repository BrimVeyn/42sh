/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 09:02:04 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/13 17:22:19 by bvan-pae         ###   ########.fr       */
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





//TODO: Delete this structure
typedef struct {
	int start;
	int end;
} Range;



//----------------Garbage-------------------//

typedef enum {
	GC_GENERAL,
	GC_ENV,
	GC_SUBSHELL,
	GC_ALL,
} type_of_garbage;

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

#define gc_unique(datatype, garbage_collector_level) \
	gc(GC_CALLOC, 1, sizeof(datatype), garbage_collector_level); \

#define da_create(name, datatype, garbage_collector_level) \
	datatype *name = gc(GC_CALLOC, 1, sizeof(datatype), garbage_collector_level); \
	(name)->data = gc(GC_CALLOC, 10, sizeof(void *), garbage_collector_level); \
	(name)->size = 0; \
	(name)->capacity = 10;

#define da_push(array, new_element, garbage_collector_level) \
	do { \
		if ((array)->size >= (array)->capacity) { \
			(array)->capacity *= 2; \
			(array)->data = gc(GC_REALLOC, (array)->data, (array)->size, (array)->capacity, sizeof(void *), garbage_collector_level); \
		} \
		(array)->data[(array)->size++] = new_element; \
	} while (0); \

#define da_push_front(array, new_element, garbage_collector_level) \
	do { \
		if ((array)->size >= (array)->capacity) { \
			(array)->capacity *= 2; \
			(array)->data = gc(GC_REALLOC, (array)->data, (array)->size, (array)->capacity, sizeof(void *), garbage_collector_level); \
		} \
		ft_memmove(&(array)->data[1], (array)->data, sizeof(void *) * (array)->size++); \
		(array)->data[0] = new_element; \
	} while (0); \

#define da_erase_index(array, index) \
	do { \
		if (index < (array)->size)  { \
			ft_memmove(&(array)->data[index], &(array)->data[index + 1], sizeof(void *) * ((array)->size - index)); \
			(array)->data[--(array)->size] = NULL; \
		} \
	} while (0); \

#define da_clear(array, garbage_collector_level) \
	do { \
		for (size_t i = 0; i < (array)->size; i++) { \
			gc(GC_FREE, (array)->data[i], garbage_collector_level); \
		} \
		ft_memset((array)->data, 0, sizeof(void *) * (array)->size); \
		(array)->size = 0; \
	} while (0); \
	
#define da_pop(array) \
	((array)->size == 0 ? NULL : (array)->data[--(array)->size]); \

#define da_print(array) _Generic((array), \
	StrList *: str_list_print, \
	StringList *: string_list_print, \
	TokenList *: tokenListToString \
)((array))

//------------------------------------------//


//----------------Utils--------------------//
void			*ft_realloc(void *ptr, size_t old_size, size_t new_size, size_t element_size);
void			free_charchar(char **array);
size_t			ft_strlenlen(const char **strstr);
char			**ft_strdupdup(const char **env);
char			*replace_char_greedy(char *str, char c, char by);
int				ft_strstr(char *haystack, char *needle);
int				ft_strrstr(char *haystack, char *needle);
void			ft_sprintf(char *buffer, const char *fmt, ...);
int				ft_snprintf(char *buffer, const size_t size_of_buffer, const char *fmt, ...);
void			ft_dprintf(int fd, const char *fmt, ...);
long			ft_atol(const char *str);
char			*ft_ltoa(long n);

//----------------File Utils--------------------//
char			*get_next_line(int fd);
char			*read_whole_file(int fd);

void			fatal(char *msg, int exit_code);

#endif // !UTILS_H
