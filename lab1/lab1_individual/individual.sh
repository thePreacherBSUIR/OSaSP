#!/bin/bash
dir=`realpath $1`
if [ $# == 4 ]
then
	find $dir -type f -size +$2c -size -$3c -printf 'file: %h/%f\nsize: %s bytes\n\n' >$4  
	echo
	echo -n 'amount of viewed files: '
	find $dir -type f | wc -l
	echo
else
	echo 'ur amount of parameters must b 4' >&2
fi
