/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 13:14:29 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/26 13:49:00 by bvan-pae         ###   ########.fr       */
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
        regex_test("\\${[A-Za-z_]*}", "echo ${PATH}");
		regex_test("\\${[^}]*}", "echo ${PATH}");
		regex_test("\\${[^}]*}", "echo ${}");
		regex_test("\\${}", "${}");
		regex_test("\\${[A-Za-z_][A-Za-z0-9_]*}", "${}");
		regex_test("\\${[A-Za-z_][A-Za-z0-9_]*}", "${PATH}");
		regex_test("\\${[A-Za-z_][A-Za-z0-9_]*}", "${8PATH}");
		regex_test("\\${[^}]*}", "${9PATH}");
		regex_test("\\${[^}]*$", "${vide");
		regex_test("\\${[^}]*$", "${dsadas}");
		regex_test("\\${[A-Za-z_][A-Za-z0-9_]*}", "${PA${PATH}");
		regex_test(".*d", "Hello World");
		regex_test(".*l$", "Hello World");
		//crotte sous le tapis pour le '-'
		regex_test("\\${[A-Za-z_][A-Za-z0-9-?_]*}", "${PWD-hello}");
		regex_test("\\${[A-Za-z_][A-Za-z0-9_]*[:#%].*}", "${PWD##/hel**lo}");
		regex_test("\\${[A-Za-z_][A-Za-z0-9_]*[:#%]*.*}", "${PWD}");
    }
    return 0;
}

