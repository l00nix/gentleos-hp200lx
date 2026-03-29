#!/bin/sh

for x in gui apps lib kernel data; do
    cproto -I include -v -e ${x}/*.c | grep -Ev '(_far|krn_heap_alloc)' | sed -e 's/$/\r/' > include/tmp.h
    mv include/tmp.h include/p_${x}.h
done
