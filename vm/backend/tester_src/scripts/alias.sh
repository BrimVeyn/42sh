alias myAlias='pwd && echo "Done !"';
alias greetOne='echo Hello'
alias greetTwo='echo Hi'
alias greetThree='echo Hello'
alias myOtherAlias='echo Fake ls'

alias
alias myAlias greetOne
unalias myAlias greetOne
alias
alias myAlias greetOne
