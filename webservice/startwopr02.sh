#!/bin/bash
PORT=7002
DATADIR=../data

wopr --verbose -r xmlserver -p ibasefile:${DATADIR}/sonar_newspapercorp_tokenized.3.txt.l0r2_-a4+D.ibase,lexicon:${DATADIR}/sonar_newspapercorp_tokenized.3.txt.lex,keep:1,mode:1,verbose:2,lb:1,port:"${PORT}",lc:0,rc:2
