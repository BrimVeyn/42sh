case "${var}" in
    1) echo "One";;
    2) echo "Two";;
    *) echo "Default case";;
esac

var=hello
case "${var}" in
    hello) echo "Greeting";;
    goodbye) echo "Farewell";;
    *) echo "Unknown";;
esac

var=a
case "${var}" in
    a|b|c) echo "Letter is a, b, or c";;
    d) echo "Letter is d";;
    *) echo "Other letter";;
esac

var=""
case "${var}" in
    "") echo "Empty string";;
    *) echo "Non-empty";;
esac

var=" "
case "${var}" in
    " ") echo "Whitespace";;
    *) echo "Non-whitespace";;
esac

var="hello world"
case "${var}" in
    "hello world") echo "Exact match";;
    *) echo "No match";;
esac


var="$(echo salut mec)"
case "${var}" in
	"salut mec") echo "Match";;
	*) echo "No Match";;
esac
#
# var="file.txt"
# case "${var}" in
#     *.txt) echo "Text file";;
#     *.sh) echo "Shell script";;
#     *) echo "Other file";;
# esac
#
# var="abc"
# case "${var}" in
#     [a-z]*) echo "Starts with a letter";;
#     [0-9]*) echo "Starts with a number";;
#     *) echo "Other";;
# esac
#
# var="123abc"
# case "${var}" in
#     *abc) echo "Ends with abc";;
#     123*) echo "Starts with 123";;
#     *) echo "No match";;
# esac
#
# var=100
# case "${var}" in
#     [0-9][0-9]) echo "Two-digit number";;
#     100) echo "Exact match";;
#     *) echo "Other";;
# esac
#
# case "${var}" in
#     "") echo "Var is empty";;
#     *${pattern}) echo "Matches pattern '${pattern}'";;
#     *) echo "No match";;
# esac
#
# var="a b c"
# case "${var}" in
#     "a b c") echo "Exact match";;
#     "a*b*c") echo "Wildcard match";;
#     *) echo "No match";;
# esac
#
# var="${other_var:-default}"
# case "${var}" in
#     default) echo "Default value used";;
#     *) echo "Value is: ${var}";;
# esac
#
# var=${unset_var:-"fallback"}
# case "${var}" in
#     fallback) echo "Fallback used";;
#     *) echo "Variable value: ${var}";;
# esac
#
# var=$(echo "test case")
# case "${var}" in
#     "test case") echo "Command substitution match";;
#     *) echo "No match";;
# esac
#
# case "${var}" in
#     ${unset_var:+set}) echo "Var is set";;
#     ${unset_var:-unset}) echo "Var is unset";;
# esac
#
# var="spaces  "
# case "${var}" in
#     "spaces  ") echo "Trailing spaces preserved";;
#     "spaces") echo "No trailing spaces";;
# esac
#
# case "${var}" in
#     $((5+5))) echo "Arithmetic expansion 10";;
#     *) echo "No arithmetic match";;
# esac
