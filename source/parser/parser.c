/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 15:39:49 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/12 08:34:59 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

bool is_operator(type_of_separator s) {
	return s == S_BG || s == S_EOF || s == S_OR || s == S_AND || s == S_SEMI_COLUMN || s == S_NEWLINE || s == S_PIPE;
}

char *here_doc(char *eof){
	char *input = NULL;
	static int heredoc_number = 0;
	char filename[50]; 
	sprintf(filename, "/tmp/here_doc_%d", heredoc_number++);
	int file_fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0664);
	if (file_fd == -1) {
		return NULL;
	}
	while((input = readline("> ")) && ft_strcmp(eof, input)){
		dprintf(file_fd, "%s\n", input);
	}
	close(file_fd);
	return gc_add(ft_strdup(filename));
}

bool heredoc_detector(TokenList *data) {
	for (uint16_t it = 0; it < data->size; it++) {
		const Token *curr = data->t[it];
		Token *const el = (curr->tag == T_WORD && curr->w_postfix->tag == T_REDIRECTION) ? curr->w_postfix : (Token *) curr;
		if (el->tag == T_REDIRECTION && el->r_type == R_HERE_DOC) {
			Token *filename = el->r_postfix;
			el->r_type = R_INPUT;
			filename->w_infix = here_doc(filename->w_infix);
			if (!filename->w_infix) return false;
		}
	}
	return true;
}

RedirectionList *parser_get_redirection(TokenList *tl) {
	RedirectionList *redir_list = redirection_list_init();
	for (uint16_t it = 0; it < tl->size; it++) {
		const Token *el = tl->t[it];
		if (el->tag == T_REDIRECTION ||
			(el->tag == T_WORD && el->w_postfix->tag == T_REDIRECTION))
		{
			const Token *next = (el->tag == T_WORD && el->w_postfix->tag == T_REDIRECTION) ? el->w_postfix : el;
			Redirection *current_redir = (Redirection *) gc_add(ft_calloc(1, sizeof(Redirection)));

			current_redir->prefix_fd = (next != el) ? ft_atoi(el->w_infix) : -1;
			current_redir->r_type = next->r_type;
			if (is_number(next->r_postfix->w_infix) &&
				(next->r_type == R_DUP_OUT ||
				next->r_type == R_DUP_IN)) {
				current_redir->su_type = R_FD;
				current_redir->fd = ft_atoi(next->r_postfix->w_infix);
			} else {
				current_redir->su_type = R_FILENAME;
				current_redir->filename = next->r_postfix->w_infix;
			}
			redirection_list_add(redir_list, current_redir);
		}
	}
	return redir_list;
}

SimpleCommand *parser_get_command(TokenList *tl) {
	SimpleCommand *curr_command = (SimpleCommand *) gc_add(ft_calloc(1, sizeof(SimpleCommand)));

	size_t count = 0;
	for (uint16_t it = 0; it < tl->size; it++) {
		const Token *el = tl->t[it];
		count += (el->tag == T_WORD && el->w_postfix->tag != T_REDIRECTION);
	}

	curr_command->args = (char **) gc_add(ft_calloc(count + 1, sizeof(char *)));

	size_t i = 0;
	for (uint16_t it = 0; it < tl->size; it++) {
		const Token *el = tl->t[it];
		if (el->tag == T_WORD && el->w_postfix->tag != T_REDIRECTION) {
			if (i == 0) {
				curr_command->bin = el->w_infix;
			}
			curr_command->args[i++] = el->w_infix;
		}
	}
	curr_command->args[i] = NULL;
	curr_command->next = NULL;
	return curr_command;
}

char *parser_get_variable_value(char *name){
	char **env = __environ;

	for (int i = 0; env[i]; i++){
		if (ft_strncmp(env[i], gc_add(ft_strjoin(name, "=")), ft_strlen(name) + 1) == 0){
			return ft_strdup(env[i] + ft_strlen(name) + 1);
		}
	}
	return ft_strdup("");
}

bool parser_parameter_expansion(TokenList *tl){
	for (uint16_t i = 0; i < tl->size; i++){
		Token *el = tl->t[i];

		if (el->tag == T_WORD){
			match_result result;
			int count = 0;
			char *value = NULL;
			int pos = -1;

			do{
				for (int i = 0; el->w_infix[i]; i++){
					if (el->w_infix[i] == '$'){
						pos = i;
						break;
					}
				}
				if (pos == -1){
					break;
				}

				if (regex_match("\\${}", el->w_infix).start == pos || //match ${} 
					regex_match("\\${[^}]*$", el->w_infix).start == pos || //if missing }
					(regex_match("\\${[^}]*}", el->w_infix).start == pos && regex_match("\\${[A-Za-z_][A-Za-z0-9_]*}", el->w_infix).start != pos)){ //check if first char is a number
					dprintf(2, "${}: bad substitution\n");
					g_exitno = 1;
					return false;
				}
				
				result = regex_match("\\${?}", el->w_infix);
				if (result.start != -1)
					value = ft_itoa(g_exitno);
				else{
					result = regex_match ("\\${[0-9a-zA-Z_]*}", el->w_infix);
					if (result.start != -1)
						value = parser_get_variable_value(gc_add(ft_substr(el->w_infix, result.start + 2, result.end - result.start - 3)));
					else
						break;
				}
				
				count++;
				char *start = ft_substr(el->w_infix, 0, result.start);
				char *end = ft_substr(el->w_infix, result.end, ft_strlen(el->w_infix));
				if (start &&
				value){}
				char *tmp = ft_strjoin(start, value);

				el->w_infix = gc_add(ft_strjoin(tmp, end));
				free(tmp); free(start); free(end); free(value);

			} while(regex_match("\\${[A-Za-z_][A-Za-z0-9_]*}", el->w_infix).start != -1);
		}
	}
	return true;
}

//echo 0 ; echo 1 | (echo 2 && echo 3) | echo 4 && echo 5
//echo 1 | (echo 2 && echo 3) | echo 4 && echo 5
//echo 1 | (echo 2 && echo 3) | echo 4
//echo -2 | echo -1 | echo 0; echo 1 | echo 2

type_of_separator next_separator(TokenList *list, int *i) {
	int j = *i;
	while (j < list->size) {
		if (list->t[j]->tag == T_SEPARATOR && 
			(list->t[j]->s_type == S_SEMI_COLUMN ||
			list->t[j]->s_type == S_PIPE)) {
			return list->t[j]->s_type;
		}
		(j)++;
	}
	return S_EOF;
}

void skip_subshell(TokenList *newlist, TokenList *list, int *i) {
	token_list_add(newlist, list->t[*i]);
	(*i)++;
	while (*i < list->size && !is_end_sub(list, i)) {
		if (is_subshell(list, i)) {
			skip_subshell(newlist, list, i);
		}
		if (*i < list->size) {
			token_list_add(newlist, list->t[*i]);
			(*i)++;
		}
	}
}

TokenList *extract_subshell_rec(TokenList *list, int *i) {
	TokenList *newlist = token_list_init();
	(*i)++;
	while (*i < list->size && !is_end_sub(list, i)) {
		if (*i < list->size && is_subshell(list, i)) {
			skip_subshell(newlist, list, i);
		}
		if (*i < list->size) {
			token_list_add(newlist, list->t[*i]);
			(*i)++;
		}
	}
	(*i) += 1;
	return newlist;
}

Node *extract_subshell(TokenList *list, int *i) {
	TokenList *newlist = extract_subshell_rec(list, i);
	(*i) += 1;
	if (g_debug > 2) {
		printf("============================================\n");
		tokenToStringAll(newlist);
		printf("============================================\n");
	}
	return ast_build(newlist);
}

TokenList *extract_tokens(TokenList *list, int *i) {
	TokenList *newlist = token_list_init();
	while (*i < list->size && !is_pipe(list, i) && !is_eof(list, i) && !is_semi(list, i)) {
		token_list_add(newlist, list->t[*i]);
		(*i)++;
	}
	(*i)++;
	return newlist;
}

ExecuterList *build_executer_list(TokenList *list) {
	// if (true){
	// 	printf(C_RED"----------Before EXECUTER-------------"C_RESET"\n");
	// 	tokenToStringAll(list); //Debug
	// }
	ExecuterList *self = executer_list_init();
	int i = 0;
	while (i < list->size) {
		if (next_separator(list, &i) == S_PIPE) {
			Executer *executer = NULL;
			while (next_separator(list, &i) == S_PIPE) {
				if (is_subshell(list, &i)) {
					Executer *new = executer_init(extract_subshell(list, &i), NULL);
					executer_push_back(&executer, new);
				} else {
					Executer *new = executer_init(NULL, extract_tokens(list, &i));
					executer_push_back(&executer, new);
				}
			}
			if (i < list->size) {
				if (is_subshell(list, &i)) {
					Executer *new = executer_init(extract_subshell(list, &i), NULL);
					executer_push_back(&executer, new);
				} else {
					Executer *new = executer_init(NULL, extract_tokens(list, &i));
					executer_push_back(&executer, new);
				}
			}
			executer_list_push(self, executer);
		}
		if (i < list->size && (next_separator(list, &i) == S_EOF || next_separator(list, &i) == S_SEMI_COLUMN)) {
			Executer *executer = NULL;
			if (is_subshell(list, &i)) {
				Executer *new = executer_init(extract_subshell(list, &i), NULL);
				executer_push_back(&executer, new);
			} else {
				Executer *new = executer_init(NULL, extract_tokens(list, &i));
				executer_push_back(&executer, new);
			}
			executer_list_push(self, executer);
		}
	}
	return self;
}

// int main() {
//     const char *dossier = ".";  // Par exemple, le répertoire courant
//     struct dirent *entry;       // Pointeur pour les entrées du répertoire
//     DIR *dir = opendir(dossier);  // Ouvre le répertoire
//
//     if (dir == NULL) {
//         // Si le répertoire n'a pas pu être ouvert
//         perror("opendir");
//         return EXIT_FAILURE;
//     }
//
//     // Tant qu'il y a des entrées dans le répertoire
//     while ((entry = readdir(dir)) != NULL) {
//         // Affiche le nom de l'entrée (fichier ou dossier)
//         printf("%s\n", entry->d_name);
//     }
//
//     // Ferme le répertoire
//     closedir(dir);
//     return EXIT_SUCCESS;
// }

char *parser_bash_to_regexp(char *str){
	char *new_str = NULL;
	for (int i = 0; str[i]; i++){
		if (str[i] == '*'){
			char *start = (i == 0) ? ft_strdup("") : ft_substr(str, 0, i);
			char *end = (i == (int)ft_strlen(str)) ? ft_strdup("") : ft_substr(str, i + 1, ft_strlen(str) - i);
			free(new_str);
			new_str = ft_calloc(ft_strlen(str) + 2, sizeof(char));
			sprintf(new_str, "%s.*%s", start, end);
			free(str);
			str = new_str;
			i += 2;
		}
	}
	return new_str;
}

int parser_filename_expansion(TokenList *tl){
	
	for (uint16_t i = 0; i < tl->size; i++) {
		const Token *el = tl->t[i];
		char *str = el->w_infix;
	
		//need to handle /*
		if ((el->tag == T_WORD || el->tag == T_REDIRECTION) && there_is_star(str)) {
			struct dirent *entry;
			DIR *dir;
			(void)entry;
			(void)dir;

			if (!there_is_slash(str)){
				dir = opendir(".");
			} else {
				printf("Bon, la j'ai la flemme je le ferais plus tard\n");
			}

			char *regexp = parser_bash_to_regexp(str);
			
			// while ((entry = readdir(dir)) != NULL) {
			// 	if (regex_match(regexp, str))
			// 	printf("%s\n", entry->d_name);
			// }
			free(regexp);
		}
	}
	return true;
}

int there_is_slash(char *str){
	for (int i = 0; str[i]; i++){
		if (str[i] == '/'){
			return 1;
		}
	}
	return 0;
}

int there_is_star(char *str){
	for (int i = 0; str[i]; i++){
		if (str[i] == '*'){
			return 1;
		}
	}
	return 0;
}

SimpleCommand *parser_parse_current(TokenList *tl) {
	
	// parser_brace_expansion();
	// parser_tilde_expansion();
	if (!parser_parameter_expansion(tl)){
		return NULL;
	}
	// parser_command_substitution();
	// parser_arithmetic_expansion();
	// parser_word_splitting();
	// parser_filename_expansion();
	// parser_quote_removal();
	RedirectionList *redirs = parser_get_redirection(tl);
	SimpleCommand *command = parser_get_command(tl);
	command->redir_list = redirs;
	
	return command;
}
