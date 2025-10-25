#!/bin/bash

git sumbodule init
git submodule update

cd vcpkg
./bootstrap-vcpkg.sh
cd ..

cmake -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build