#!/bin/sh

for x in gui apps lib kernel data; do
    cproto -D __CPROTO__ -I include -v -e ${x}/*.c | sed -e 's/$/\r/' > include/tmp.h
    mv include/tmp.h include/p_${x}.h
done
