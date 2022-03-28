#!/bin/bash
if [[ $# == 3 ]]
then
	if [[ ! -f "$1" || ! -d "$2" ]] 
	then	{
			echo "ur 1st argument must b a file"
			echo "ur 2nd argument must b a directory"
			echo "ur 3rd argument must b a type of file"
		} 	>&2
	else
		find "$2" -type f -name "*.$3" | sort >$1
	fi
else
	echo "ur amount of parameters must b 3" >&2
fi
