/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   c_string.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 17:32:48 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/10 14:04:19 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef C_STRING_H
# define C_STRING_H

#include <stdio.h>

typedef struct {
	char *data;
	size_t size;
	size_t capacity;
} string;

#define LOREM "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ornare aenean euismod elementum nisi. Auctor augue mauris augue neque gravida in fermentum et. Quis viverra nibh cras pulvinar. Facilisi nullam vehicula ipsum a arcu cursus vitae congue mauris. Tempus quam pellentesque nec nam aliquam sem et. Urna id volutpat lacus laoreet non curabitur. Pharetra diam sit amet nisl suscipit adipiscing bibendum est ultricies. Ornare arcu odio ut sem nulla. Bibendum at varius vel pharetra vel turpis nunc eget. Viverra tellus in hac habitasse platea. In nulla posuere sollicitudin aliquam ultrices sagittis orci a scelerisque. Mattis vulputate enim nulla aliquet porttitor lacus luctus accumsan tortor. Mauris nunc congue nisi vitae suscipit tellus mauris a. Vulputate sapien nec sagittis aliquam malesuada bibendum arcu.Lacus laoreet non curabitur gravida arcu ac. Massa sed elementum tempus egestas sed sed risus pretium quam. Nisl nisi scelerisque eu ultrices vitae auctor eu. Ac orci phasellus egestas tellus rutrum. Commodo viverra maecenas accumsan lacus vel facilisis volutpat. Viverra tellus in hac habitasse platea dictumst vestibulum. Sit amet consectetur adipiscing elit ut aliquam purus. Sapien pellentesque habitant morbi tristique senectus et netus et. Amet tellus cras adipiscing enim eu turpis. Fermentum et sollicitudin ac orci.Elit eget gravida cum sociis natoque penatibus et. Nibh nisl condimentum id venenatis a. Bibendum ut tristique et egestas quis ipsum suspendisse. Tristique sollicitudin nibh sit amet commodo nulla facilisi. Nunc mattis enim ut tellus elementum sagittis. Fermentum leo vel orci porta non pulvinar neque laoreet suspendisse. Ut aliquam purus sit amet luctus. Vitae aliquet nec ullamcorper sit amet risus. Urna cursus eget nunc scelerisque viverra. Scelerisque viverra mauris in aliquam sem fringilla ut morbi. Odio pellentesque diam volutpat commodo sed egestas. Pellentesque elit eget gravida cum sociis natoque penatibus. Id consectetur purus ut faucibus. Ac tortor vitae purus faucibus ornare suspendisse sed nisi. Quis commodo odio aenean sed adipiscing diam. Eget velit aliquet sagittis id.Enim nec dui nunc mattis enim ut tellus elementum sagittis. Quam nulla porttitor massa id neque aliquam. Venenatis tellus in metus vulputate eu scelerisque felis imperdiet. Convallis convallis tellus id interdum. Platea dictumst quisque sagittis purus sit amet. Elementum sagittis vitae et leo duis ut diam quam. Nam libero justo laoreet sit amet cursus sit amet dictum. Accumsan tortor posuere ac ut consequat semper viverra. Elementum facilisis leo vel fringilla est ullamcorper eget nulla facilisi. Ut consequat semper viverra nam libero justo. Rhoncus mattis rhoncus urna neque viverra justo nec ultrices. Amet est placerat in egestas erat imperdiet sed euismod nisi.Morbi quis commodo odio aenean sed adipiscing diam donec adipiscing. Adipiscing elit duis tristique sollicitudin nibh sit amet commodo. Tristique risus nec feugiat in fermentum posuere urna. Non diam phasellus vestibulum lorem. Nibh cras pulvinar mattis nunc sed blandit. Eu augue ut lectus arcu bibendum at varius vel. Donec et odio pellentesque diam volutpat commodo sed. Integer malesuada nunc vel risus commodo viverra maecenas. Sed risus pretium quam vulputate. Enim nec dui nunc mattis enim ut tellus. Nec sagittis aliquam malesuada bibendum arcu. Tortor at risus viverra adipiscing at. Ut enim blandit volutpat maecenas volutpat blandit aliquam etiam erat. Etiam sit amet nisl purus in. Fames ac turpis egestas sed tempus. Ut diam quam nulla porttitor massa id. Pellentesque massa placerat duis ultricies lacus sed turpis. Magna etiam tempor orci eu lobortis elementum nibh tellus molestie. Sagittis vitae et leo duis ut diam quam nulla porttitor. Aliquet eget sit amet tellus cras."
#define LOREM2 "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ornare aenean euismod elementum nisi. Auctor augue mauris augue neque gravida in fermentum et. Quis viverra nibh cras pulvinar. Facilisi nullam vehicula ipsum a arcu cursus vitae congue mauris. Tempus quam pellentesque nec nam aliquam sem et. Urna id volutpat lacus laoreet non curabitur. Pharetra diam sit amet nisl suscipit adipiscing bibendum est ultricies. Ornare arcu odio ut sem nulla. Bibendum at varius vel pharetra vel turpis nunc eget. Viverra tellus in hac habitasse platea. In nulla posuere sollicitudin aliquam ultrices sagittis orci a scelerisque. Mattis vulputate enim nulla aliquet porttitor lacus luctus accumsan tortor. Mauris nunc congue nisi vitae suscipit tellus mauris a. Vulputate sapien nec sagittis aliquam malesuada bibendum arcu.Lacus laoreet non curabitur gravida arcu ac. Massa sed elementum tempus egestas sed sed risus pretium quam. Nisl nisi scelerisque eu ultrices vitae auctor eu. Ac orci phasellus egestas tellus rutrum. Commodo viverra maecenas accumsan lacus vel facilisis volutpat. Viverra tellus in hac habitasse platea dictumst vestibulum. Sit amet consectetur adipiscing elit ut aliquam purus. Sapien pellentesque habitant morbi tristique senectus et netus et. Amet tellus cras adipiscing enim eu turpis. Fermentum et sollicitudin ac orci.Elit eget gravida cum sociis natoque penatibus et. Nibh nisl condimentum id venenatis a. Bibendum ut tristique et egestas quis ipsum suspendisse. Tristique sollicitudin nibh sit amet commodo nulla facilisi. Nunc mattis enim ut tellus elementum sagittis. Fermentum leo vel orci porta non pulvinar neque laoreet suspendisse. Ut aliquam purus sit amet luctus. Vitae aliquet nec ullamcorper sit amet risus. Urna cursus eget nunc scelerisque viverra. Scelerisque viverra mauris in aliquam sem fringilla ut morbi. Odio pellentesque diam volutpat commodo sed egestas. Pellentesque elit eget gravida cum sociis natoque penatibus. Id consectetur purus ut faucibus. Ac tortor vitae purus faucibus ornare suspendisse sed nisi. Quis commodo odio aenean sed adipiscing diam. Eget velit aliquet sagittis id.Enim nec dui nunc mattis enim ut tellus elementum sagittis. Quam nulla porttitor massa id neque aliquam. Venenatis tellus in metus vulputate eu scelerisque felis imperdiet. Convallis convallis tellus id interdum. Platea dictumst quisque sagittis purus sit amet. Elementum sagittis vitae et leo duis ut diam quam. Nam libero justo laoreet sit amet cursus sit amet dictum. Accumsan tortor posuere ac ut consequat semper viverra. Elementum facilisis leo vel fringilla est ullamcorper eget nulla facilisi. Ut consequat semper viverra nam libero justo. Rhoncus mattis rhoncus urna neque viverra justo nec ultrices. Amet est placerat in egestas erat imperdiet sed euismod nisi.Morbi quis commodo odio aenean sed adipiscing diam donec adipiscing. Adipiscing elit duis tristique sollicitudin nibh sit amet commodo. Tristique risus nec feugiat in fermentum posuere urna. Non diam phasellus vestibulum lorem. Nibh cras pulvinar mattis nunc sed blandit. Eu augue ut lectus arcu bibendum at varius vel. Donec et odio pellentesque diam volutpat commodo sed. Integer malesuada nunc vel risus commodo viverra maecenas. Sed risus pretium quam vulputate. Enim nec dui nunc mattis enim ut tellus. Nec sagittis aliquam malesuada bibendum arcu. Tortor at risus viverra adipiscing at. Ut enim blandit volutpat maecenas volutpat blandit aliquam etiam erat. Etiam sit amet nisl purus in. Fames ac turpis egestas sed tempus. Ut diam quam nulla porttitor massa id. Pellentesque massa placerat duis ultricies lacus sed turpis. Magna etiam tempor orci eu lobortis elementum nibh tellus molestie. Sagittis vitae et leo duis ut diam quam nulla porttitor. Aliquet eget sit amet tellus cras ."

// ************************************************
// *                   capacity                   *
// ************************************************

int str_length(const string *str);
void str_resize(string *str, size_t size);
int str_capacity(const string *str);
void str_reserve(string *str, size_t reserve);
void str_clear(string *str);
int str_empty(string *str);
void str_shrink_to_fit(string *str);

// ************************************************
// *                 modifiers                    *
// ************************************************

void str_append(string *str1, string *str2);
void str_erase(string *str, size_t pos, size_t len);
void str_insert_str(string *str, const char *str2, size_t i);
void str_insert(string *str, const char c, size_t i);
void str_pop_back(string *str);
void str_pop_front(string *str);
void str_push_back(string *str, size_t c);

// ************************************************
// *           string operations                  *
// ************************************************

char *str_data(const string *str);
size_t str_find(const string *str, const char *to_find);
size_t str_find_first_of(const string *str, size_t c);
size_t str_find_last_of(const string *str, size_t c);
size_t str_rfind(const string *str, const char *to_find);
string str_substr(string *str, size_t pos, size_t len);
int str_compare(char *s1, char *s2);
size_t str_find_simd(const string *str, const char *to_find);
int str_compare_simd(string *s1, string *s2);

// ************************************************
// *                    utils                     *
// ************************************************

void str_assign_right_size(string *str, size_t len);
string string_init_str(const char *str);
void str_destroy(string *str);
string str_dup(const string *str);

#define STRING_L(value) string_init_str(value);

#endif // C_STRING_H
