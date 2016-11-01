#!/bin/bash

for file in test/*.oc; do
	echo "running $file"
	./occ -d=$file.dbg $file
	#cat $file | ./compiler $file.out
done
