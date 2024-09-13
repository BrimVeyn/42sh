let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd ~/Desktop/42cursus/42sh_git
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
let s:shortmess_save = &shortmess
if &shortmess =~ 'A'
  set shortmess=aoOA
else
  set shortmess=aoO
endif
badd +137 source/main.c
badd +202 source/parser/parser_command_substitution.c
badd +2 tester/command_sub
badd +181 source/exec/exec.c
badd +18 source/utils/utils_string.c
badd +51 include/42sh.h
badd +17 source/lexer/lexer_main.c
badd +29 source/ast/ast_main.c
badd +48 source/debug/debug_parser.c
badd +259 source/parser/parser.c
badd +34 source/lexer/lexer_init.c
badd +21 libftprintf/sources/libft/ft_strchr.c
badd +20 source/lexer/TokenList.c
badd +97 source/lexer/lexer.h
badd +66 source/debug/debug_lexer.c
badd +50 source/ast/ast.h
badd +43 source/lexer/lexer_syntax.c
badd +27 source/parser/parser.h
argglobal
%argdel
edit source/lexer/TokenList.c
argglobal
balt libftprintf/sources/libft/ft_strchr.c
setlocal fdm=manual
setlocal fde=0
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
silent! normal! zE
let &fdl = &fdl
let s:l = 20 - ((19 * winheight(0) + 22) / 45)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 20
normal! 0
tabnext 1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0 && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20
let &shortmess = s:shortmess_save
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
set hlsearch
nohlsearch
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
