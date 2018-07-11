# The RebirthDB CMAKE build system

This build system has been tested on an Ubuntu 14.04 x86_64 machine.
It does not support cross-compilation or packaging yet.

## Steps to build RebirthDB
The following instructions are specific to Ubuntu 14.04 and are yet
 to be tested on other operating systems.
 
#### 1. Get the build dependencies

```bash
sudo apt-get install build-essential python wget curl m4 git ccache
```

#### 2. Install cmake 3.10.3
```bash
wget https://cmake.org/files/v3.10/cmake-3.10.3.tar.gz
tar -xzvf cmake-3.10.3.tar.gz
cd cmake-3.10.3/
./bootstrap -- -DCMAKE_BUILD_TYPE:STRING=Release
make
sudo make install
```
If you already have the default version of cmake (2.8) that
ubuntu 14.04 ships, run this step. Otherwise, skip it.
```bash
sudo update-alternatives --install /usr/bin/cmake cmake /usr/local/bin/cmake 1 --force
```
Confirm the installation was successful
```bash
cmake --version
```
The result of `cmake --version` should be:
```text
cmake version 3.10.3

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

#### 3. Get RebirthDB's source code from Github
Cmake support will be kept on the `cmake` branch until all of the current GNU Make build
system's functionality have been ported to cmake.
```bash
git clone https://github.com/RebirthDB/rebirthdb.git
cd rebirthdb
git checkout cmake
```

#### 4. Set up the build directories
Create a `cmake-build-debug` directory for **debug** builds and `cmake-build-release` 
directory for **release** builds.
```bash
mkdir cmake-build-debug cmake-build-release
```

#### 5. Generate the cmake build trees and build the RebirthDB binaries
There are two main targets i.e `rebirthdb` and `rebirthdb-unittest`. Set the cmake options
`ON` or `OFF` as suitable.<br>
For a **debug** build:
```bash
cd cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=Debug -DVERBOSE=ON -DDEPENDS_DEBUG_MODE=ON ..
```
- `-DVERBOSE=ON` will generate verbose makefiles and run the build in verbose mode.
- `-DDEPENDS_DEBUG_MODE=ON` will print the dependency list of the project's different dependencies
at the end of the configuration stage.

Both options are `OFF` by default.

The above command will generate the project's build tree and makefiles.
To perform the actual builds for the different targets, run:
##### 1. rebirthdb
```bash
cmake --build . --target rebirthdb -- -j 4 ..
```
##### 2. rebirthdb-unittest
```bash
cmake --build . --target rebirthdb-unittest -- -j 4 ..
```
- `--build .` tells cmake to perform an in-place build.
- `--target` tells cmake which target it should build against.
- `-j 4` sets the build to run on 4 cores. Set the number of cores as desired/available.

For a **release** build:
```bash
cd cmake-build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target rebirthdb -- -j 4 ..
cmake --build . --target rebirthdb-unittest -- -j 4 ..
```

***NOTE:*** If `ccache` is installed, the build will be cached so that consecutive builds
are faster. Sometimes, if the build is cancelled mid-way, it may result into a corrupted
object file that will be cached, and thus presenting problems at the linking stage.
Should you experience such a problem, run `ccache -C` (capital C) to clear the cache
completely, before proceeding with the build commands.
