#!/bin/sh

if ! ls | grep -q "build.sh"; then
    echo "You must be in the root folder of the project"
    exit 1
fi

cd Toolchain
make all
cd ..

if [ $# -eq 1 ]; then
    if [ $1 = "buddy" ]; then
	    make buddy
    elif [ $1 = "spanish" ]; then
        make spanish
    else
	    echo "Do you want to compile with the buddy memory manager? if so, you must pass buddy as argument"
    fi
elif [ $# -eq 0 ]; then
    make all
else
    echo "You can only enter one parameter or none"
fi


