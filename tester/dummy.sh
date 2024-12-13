#/bin/bash
echo $(/usr/bin/echo -e 'echo ${VAR:?salope}' | bash)
echo $(/usr/bin/echo -e 'echo ${VAR:?salope}' | ../42sh)
/usr/bin/echo -e 'salope'
