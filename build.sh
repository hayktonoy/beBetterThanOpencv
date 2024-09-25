#!/usr/bin/env bash

cmd_list="$1"

mkdir -p build
pushd build 2>/dev/null

	echo "$cmd_list" | fold -w 1 | while read cmd
	do
		if [ "$cmd" = 'q' ]
		then
			qmake CONFIG+=debug ..
			compiledb -n make
			sed -i 's/"-mno-direct-extern-access",//' compile_commands.json
		elif [ "$cmd" = 'b' ]
		then
			make -j$(nproc)
		elif [ "$cmd" = 'r' ]
		then
			./beBetterThanCV
		elif [ "$cmd" = 'c' ]
		then
			make clean
		fi

		[ $? -ne 0 ] && break
	done

popd 2>/dev/null
