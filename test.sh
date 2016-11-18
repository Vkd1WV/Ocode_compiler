#!/bin/bash

for file in test/*.oc; do
	echo -e "\nrunning $file\n"
	./occ -vd $file.dbg $file
	#cat $file | ./compiler $file.out
done
