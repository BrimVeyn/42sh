#!/bin/bash

EXEC_PATH="../42sh"
SCRIPTS_DIR="./scripts"

for file in $(ls ${SCRIPTS_DIR}); do
	echo ${SCRIPTS_DIR}/${file}
	${EXEC_PATH} ${SCRIPTS_DIR}/${file} </dev/null >>d1
	bash ${SCRIPTS_DIR}/${file} </dev/null >>d2
done

diff d1 d2; rm d1 d2
