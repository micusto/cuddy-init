#!/bin/bash

#
# IMPORTANT NOTICE:
# THIS SCRIPT IS ONLY TESTET ON macOS
#

# check if docker is installed
if [ $(which docker > /dev/null 2>&1; echo $?) -ne 0 ]; then
    echo "you must have docker installed to use this script"
    exit 1;
fi

# set vars
VERSION=`cat VERSION`
BIN_NAME="cuddy-init"
GOOS=linux
GOARCH=amd64
BIN_PATH="bin/$VERSION/$BIN_NAME-$GOOS-$GOARCH"
BUILD_IMAGE="urlund/gcc-debian:jessie"

# make sure the build image exists
docker build -t $BUILD_IMAGE . > /dev/null 2>&1

# make sure the version bin directory exists
mkdir -p bin/$VERSION/

# replace version with the actual number
sed -i '' 's/Cuddy INIT, v###VERSION###/Cuddy INIT, v'"$VERSION"'/' cuddy-init.c

# build the bin inside a container
docker run --rm -v "$PWD":/work -w /work $BUILD_IMAGE gcc -o $BIN_PATH cuddy-init.c

# build MacOS binary
gcc -o bin/$VERSION/$BIN_NAME-darwin-amd64 cuddy-init.c

# replace the actual number with version text for next time
sed -i '' 's/Cuddy INIT, v'"$VERSION"'/Cuddy INIT, v###VERSION###/' cuddy-init.c
