/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dynamic_arrays.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 23:32:48 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/29 23:42:46 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DA_H
#define DA_H

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

#define da_insert(array, new_element, index) \
	do { \
		if ((index) > (array)->size) \
			break; \
		if ((array)->size >= (array)->capacity) { \
			(array)->capacity *= 2; \
			(array)->data = gc(GC_REALLOC, (array)->data, (array)->size, (array)->capacity, (array)->size_of_element, (array)->gc_level); \
		} \
		ft_memmove(&(array)->data[index + 1], &(array)->data[index], (array)->size_of_element * ((array)->size - index)); \
		(array)->data[index] = new_element; \
		(array)->size++; \
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
			ft_memmove(&(array)->data[index], &(array)->data[index + 1], (array)->size_of_element * ((array)->size - index - 1)); \
			(array)->data[--(array)->size] = 0; \
		} \
	} while (0); \

#define da_delete_index(array, index) \
	do { \
		if (index < (array)->size)  { \
			gc(GC_FREE, (array)->data[index], (array)->gc_level); \
			ft_memmove(&(array)->data[index], &(array)->data[index + 1], (array)->size_of_element * ((array)->size - index - 1)); \
			(array)->data[--(array)->size] = 0; \
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
		StringStream *: ss_pop_front \
	)(array)

#endif // !DA_H
