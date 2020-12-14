#!/bin/sh
export PORT=7003
export TIMEOUT=300000
export MEMLIMIT=1500M
export TMPDIR=/tmp
export TCLLIBPATH=${ALPINO_HOME}/create_bin/tcl8.5
export TCL_LIBRARY=${ALPINO_HOME}/create_bin/tcl8.5

## this one is for parsing

PROLOGMAXSIZE=${MEMLIMIT} ${ALPINO_HOME}/bin/Alpino -notk -veryfast user_max="${TIMEOUT}"\
 server_kind=parse\
 server_port="${PORT}"\
 assume_input_is_tokenized=on\
 debug=1\
 end_hook=xml\
-init_dict_p\
 batch_command=alpino_server 2> ${TMPDIR}/alpino_server.log
