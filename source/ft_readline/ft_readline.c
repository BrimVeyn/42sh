/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_readline.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 11:26:40 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/10 17:21:29 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_readline.h"
#include "42sh.h"
#include "ft_regex.h"
#include "c_string.h"
#include "libft.h"
#include <linux/limits.h>
#include <stdio.h>
#include <termios.h>
#include <termcap.h>
#include <unistd.h>

void enable_raw_mode()
{
    struct termios raw;

    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode()
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void move_cursor(int x, int y) {
    char buffer[2048];
    char *area = buffer;
    char *move_cursor_seq = tgetstr("cm", &area);
	
    if (move_cursor_seq) {
        char *cursor_position = tgoto(move_cursor_seq, x, y);
		tputs(cursor_position, 1, putchar);
		fflush(stdout);
    }
}

void get_cursor_pos(size_t *x, size_t *y){
    char buf[32];
    unsigned int i = 0;
	(void) y;
	(void) x;
	
	// disable_raw_mode();
    write(STDOUT_FILENO, "\033[6n", 4);

    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1 || buf[i] == 'R') {
            break;
        }
        i++;
    }
    buf[i] = '\0';
    if (buf[0] == '\033' && buf[1] == '[') {
		regex_match_t result = regex_match("\\[[0-9]*;", buf);
		char *tmp_y = ft_substr( buf, result.re_start + 1, result.re_end - result.re_start);
		*y = ft_atoi(tmp_y);
		free(tmp_y);
		
		result = regex_match(";[0-9]*$", buf);
		char *tmp_x = ft_substr( buf, result.re_start + 1, result.re_end - result.re_start);
		*x = ft_atoi(tmp_x);
		free(tmp_x);
	}
}

void update_line(const char *prompt, string *line){
	write(1, "\033[2K\r", 5);
	write(1, prompt, ft_strlen(prompt));
	write(1, line->data, str_length(line));
}

void init_readline(char *prompt, size_t *cursor_x, size_t *cursor_y) {
    static bool history_defined = false;

    char buffer[2048];
    tgetent(buffer, getenv("TERM"));

    if (!history_defined) {
        init_history();
        history_defined = true;
    }

    history->offset = history->length;
    add_history("");
    
    enable_raw_mode();
    get_cursor_pos(cursor_x, cursor_y);
    (*cursor_y)--;
    *(cursor_x) = ft_strlen(prompt);
    write(STDOUT_FILENO, prompt, ft_strlen(prompt));

	// move_cursor(1, 39);
	// printf("cursor_x: %zu, cursor_y: %zu\n", *cursor_x, *cursor_y);
	// // printf("line length: %d\nprompt lenght: %zu\n", str_length(line), ft_strlen(prompt));
	// fflush(stdout);

    move_cursor(*cursor_x, *cursor_y);
}

int handle_normal_keys(char *prompt, char c, string *line, size_t *cursor_x){
	//place new char
	if (c == '\n'){
		write(1, "\n", 1);
		// str_push_back(line, '\n');
		// update_line(prompt, line);
		write(1, "\033[2K\r", 5);
		write(1, prompt, ft_strlen(prompt));
		write(1, line->data, str_length(line));
		return 1;
	}
	if (c == 127 && *cursor_x == ft_strlen(prompt)){
		return 2;
	}

	if (*cursor_x - ft_strlen(prompt) == (size_t)str_length(line))
		if (c == 127){
			str_pop_back(line);
			*cursor_x -= 2;
		}
		else
			str_push_back(line, c);
	else
		if (c == 127){
			str_erase(line, *cursor_x - ft_strlen(prompt), 1);
			*cursor_x -= 2;
		}
		else
			str_insert(line, c, *cursor_x - ft_strlen(prompt));
	(*cursor_x)++;
	return 0;
}

int handle_special_keys(char *prompt, string *line, size_t *cursor_x, size_t *cursor_y){

	char seq[2];
	if (read(STDIN_FILENO, &seq[0], 1) == 0) return 1;
	if (read(STDIN_FILENO, &seq[1], 1) == 0) return 1;

	if (seq[0] == '['){
		if (seq[1] == 'A' && history->offset > 0){
			history->offset--;
			*cursor_x = history->entries[history->offset]->line.size + ft_strlen(prompt);
			history->entries[history->length - 1]->line = str_dup(&history->entries[history->offset]->line);
			return 2;
		}
		else if (seq[1] == 'D'){
			if (*cursor_x > ft_strlen(prompt)){
				move_cursor(--(*cursor_x), *cursor_y);
			}
		}
		else if (seq[1] == 'C'){
			if (*cursor_x <= (size_t)str_length(line) + ft_strlen(prompt) - 1){
				move_cursor(++(*cursor_x), *cursor_y);
			}
		}
		else if (seq[1] == 'B' && history->offset < history->length){
			history->offset++;
			*cursor_x = history->entries[history->offset]->line.size + ft_strlen(prompt);
			history->entries[history->length - 1]->line = str_dup(&history->entries[history->offset]->line);
			return 2;
		}
	}
	return 0;
}

char *ft_readline(char *prompt){
	char c;
	string *line = NULL;
	int result = 0;

	size_t cursor_x = 0;
	size_t cursor_y = 0;
	
    init_readline(prompt, &cursor_x, &cursor_y);
	// printf("cursor_x: %zu, cursor_y: %zu\n", cursor_x, cursor_y);

	line = &history->entries[history->length - 1]->line;
	while(true){
		update_line(prompt, line);
		// move_cursor(1, 39);
		// printf("cursor_x: %zu, cursor_y: %zu\n", cursor_x, cursor_y);
		// printf("line length: %d\nprompt lenght: %zu\n", str_length(line), ft_strlen(prompt));
		// fflush(stdout);
		move_cursor(cursor_x, cursor_y);

		// str_info(line);
		read(STDIN_FILENO, &c, 1);

		if (c == VEOF){
			write(1, "\n", 1);
			if (line->data[0] == '\0'){
				pop_history();
			}
			return NULL;
		}
		//handle specials keys
		else if (c == '\033'){
			result = handle_special_keys(prompt, line, &cursor_x, &cursor_y);
		}
		else {
			result = handle_normal_keys(prompt, c, line, &cursor_x);
		}
		if (result == 1) break;
	}
	disable_raw_mode();
	// destroy_history();
	// move_cursor(1, 38);
	// printf("done\n");
	// fflush(stdout);
	char *str = ft_strdup(line->data);
	pop_history();
	return str;
}
