#!/bin/bash


if [ -d $1 -a -f $2 ]; then
	BOEHMDIR=$1
	SRC=$2
else
	echo "usage: boehmgc-dir srcfile"
	exit 1
fi

clang -c ./libzen/lib/ZStdLib.c -o ZStdLib.o
if [ $? -ne 0 ]; then
	exit 1
fi
clang -c ./libzen/lib/ZArray.c -o ZArray.o -I$BOEHMDIR/include
if [ $? -ne 0 ]; then
	rm -f ZStdLib.o
	exit 1
fi
llc $2 -filetype=obj -o _main.o
if [ $? -ne 0 ]; then
	rm -f ZStdLib.o ZArray.o
	exit 1
fi
clang _main.o ZStdLib.o ZArray.o -L$BOEHMDIR/lib -lgc -o a.out
if [ $? -ne 0 ]; then
	rm -f ZStdLib.o ZArray.o _main.o
	exit 1
fi
LD_LIBRARY_PATH=$BOEHMDIR/lib ./a.out
if [ $? -ne 0 ]; then
	rm -f ZStdLib.o ZArray.o _main.o a.out
	exit 1
else
	rm -f ZStdLib.o ZArray.o _main.o a.out
	exit 0
fi
