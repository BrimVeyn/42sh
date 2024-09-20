/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   filename_expansion.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 13:42:21 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/12 15:32:58 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


// GROS CHANTIER ATTENTION

// int depth = 0;
//
// void remove_multiple_char(char *str, char c){
// 	int save = -1;
//
// 	for (int i = 0; str[i]; i++){
// 		if (str[i] == c && save == -1)
// 			save = i;
// 		else if (str[i] != c && save != -1){
// 			ft_memmove(&str[save + 1], &str[i], i - save - 1);
// 		}
// 	}
// }
//
// char *parser_bash_to_regexp(char *str){
//
// 	char *tmp_str = ft_calloc(ft_strlen(str) + 3, sizeof(char));
// 	if (str[0] != '*'){
// 		sprintf(tmp_str, "^%s", str);
// 		str = tmp_str;
// 	}
// 	if (str[ft_strlen(str)] != '*'){
// 		sprintf(tmp_str, "%s$", str);
// 		str = tmp_str;
// 	}
// 	
// 	remove_multiple_char(str, '*');
// 	char *new_str = NULL;
// 	for (int i = 0; str[i]; i++){
// 		if (str[i] == '*'){
// 			char *start = (i == 0) ? ft_strdup("") : ft_substr(str, 0, i);
// 			char *end = (i == (int)ft_strlen(str)) ? ft_strdup("") : ft_substr(str, i + 1, ft_strlen(str) - i);
// 			
// 			new_str = ft_calloc(ft_strlen(str) + 3, sizeof(char));
// 			sprintf(new_str, "%s.*%s", start, end);
//
// 			free(start); free(end);
//
// 			str = new_str;
// 			i++; // ????
// 		}
// 	}
// 	return new_str;
// }

// void match_filename_and_add_to_tl(){
// 	struct dirent *entry;
// 	int j = 0;
//
// 	while ((entry = readdir(dir)) != NULL){
// 		if (regex_match(regexp, entry->d_name).start != -1){
// 			if (j > 1){
// 				Token *new_token = token_empty_init();
// 				token_word_init(new_token);
//
// 				new_token->tag = T_WORD;
// 				new_token->w_infix = ft_strdup(entry->d_name);
// 				token_list_insert(tl, new_token, i);
// 			}
// 			else {
// 				tl->t[i]->w_infix = ft_strdup(entry->d_name);
// 			}
// 			j++;
// 		}
// 	}
// 	if (j == 0){
// 		token_list_remove(tl, i);
// 	}
// }

// char *get_regexp_sequence(char *regexp){
// 	int i = 0;
// 	int cmp = 0;
// 	depth++;
// 	// need to get a right formated regexp string (no///*/*/////)
//
// 	while(regexp[i] && regexp[i] == '/')
// 		i++;
// 	
// 	// implemente '//'
// 	int j = i;
// 	for (; regexp[j]; j++){
// 		if (regexp[j] == '/' && cmp == depth){
// 			char *return_value = ft_substr(regexp, i - 1, j);
// 			return return_value;
// 		}
// 		else if (regexp[j] == '/'){
// 			cmp++;
// 		}
// 	}
// 	return ft_substr(regexp, i - 1, j);
// }

// void match_filename_r(TokenList *tl, char *path, char *regexp){
// 	struct dirent *entry;
// 	DIR *dir = opendir(path);
// 	
// 	while ((entry = readdir(dir)) != NULL){
// 		if (regex_match(get_regexp_sequence(regexp), entry->d_name).start != -1){
// 			if (j > 1){
// 				Token *new_token = token_empty_init();
// 				token_word_init(new_token);
//
// 				new_token->tag = T_WORD;
// 				new_token->w_infix = ft_strdup(entry->d_name);
// 				token_list_insert(tl, new_token, i);
// 			}
// 			else {
// 				tl->t[i]->w_infix = ft_strdup(entry->d_name);
// 			}
// 			j++;
// 		}
// 	}
// 	if (j == 0){
// 		token_list_remove(tl, i);
// 	}
// 	closedir(dir);
// }

// int parser_filename_expansion(TokenList *tl){
// 	(void)tl;
// 	printf("string: %s\n", get_regexp_sequence("///*/file"));
// 	return 0;
// }

// int parser_filename_expansion(TokenList *tl){
// 	for (uint16_t i = 0; i < tl->size; i++) {
// 		const Token *el = tl->t[i];
// 		char *str = el->w_infix;
//
// 		//need to handle /*
// 		if ((el->tag == T_WORD || el->tag == T_REDIRECTION) && there_is_star(str)) {
// 			DIR *dir = NULL;
//
// 			if (!there_is_slash(str)){
// 				dir = opendir(".");
// 			} else {
// 				printf("Bon, la j'ai la flemme je le ferais plus tard\n");
// 			}
//
// 			char *regexp = parser_bash_to_regexp(str);
// 			printf("regexp: %s\n", regexp);
//
// 			closedir(dir);
// 			free(regexp);
// 		}
// 	}
// 	// tokenToStringAll(tl);
// 	return true;
// }

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
