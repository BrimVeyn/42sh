/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 13:14:29 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/03 13:15:02 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <readline/readline.h>
#include <readline/history.h>
#include "regex.h"

int main(int ac, char **av) {
    if (ac >= 2 && !strcmp(av[1], "-i")){
        char *input = NULL;
        char *string = readline("enter string\n> ");
        while((input = readline("> ")) != NULL){
            regex_test(input, string);
            add_history(input);
        }
    } else{
        char *text = "Heeello-world! 0123456789";
        regex_test("Heeello", text);
        regex_test("Hedllo", text);
        regex_test("Hi*", text);
        regex_test("He*", text);
        regex_test("He*l", text);
        regex_test("ee", text);
        regex_test("^ee", text);
        regex_test("^He", text);
        regex_test("H[A-Z]", text);
        regex_test("H[a-z]", text);
        regex_test("Heee[a-z]", text);
        regex_test("H[a-z]*", text);
        regex_test("[4-8]*", text);
        regex_test("[a-z4-8]*", "000aaaaa44444");
        regex_test("[a-z ]*", "Hello world!");
        regex_test("^[a-z ]*", "Hello world!");
        regex_test("^H[a-z-]*", "Hello-world!");
        regex_test("^H[a-z]*\\sw", "Hello world!");
        regex_test("${[A-Za-z_]*}", "echo ${PATH}");
		regex_test("${[^}]*}", "echo ${PATH}");
    }
    return 0;
}

