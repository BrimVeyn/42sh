from bs4 import BeautifulSoup

c_code = ""
case_number = 0

with open("color_samples.c", "w") as f:
    c_code += "int main(void) {\n"
    with open("include/colors.h", "r") as input:
        for line in input:
            line.replace('\t', ' ')
            words = line.split(' ')
            words = list(filter(None, words))
            if len(words) >= 3 and words[0] == '#define':
                c_code += f'printf({words[2][:-1]}"THIS A DUMMY SENTENCE\\n");\n'
    f.write(c_code)
