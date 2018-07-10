#!/usr/bin/env bash
set -ev

wget https://cmake.org/files/v3.10/cmake-3.10.3.tar.gz
tar -xzvf cmake-3.10.3.tar.gz
cd cmake-3.10.3/
./bootstrap -- -DCMAKE_BUILD_TYPE:STRING=Release
make
sudo make install
sudo update-alternatives --install /usr/bin/cmake cmake /usr/local/bin/cmake 1 --force
