from bs4 import BeautifulSoup

c_code = ""
case_number = 0

with open("color_samples.c", "w") as f:
    c_code += "#include <stdio.h>\n"
    c_code += "int main(void) {\n"
    with open("include/colors.h", "r") as input:
        for line in input:
            line.replace('\t', ' ')
            words = line.split(' ')
            words = list(filter(None, words))
            print(words)
            if len(words) >= 3 and words[0] == '#define':
                if words[2][:-1] != '\"':
                    words[2] +=  '\"'
                c_code += f'printf({words[2][:-1]}"THIS A DUMMY SENTENCE IN <-- {words[1]}\\n");\n'
        c_code += "}"
    f.write(c_code)
