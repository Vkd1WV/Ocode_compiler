#!/bin/bash

for file in test/*.sxy; do
	echo "running $file"
	cat $file | ./compiler $file.out
	
	
done
