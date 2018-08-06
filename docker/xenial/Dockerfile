FROM ubuntu:16.04

ENV DEBIAN_FRONTEND noninteractive

# Install requisite packages
# For xenial, `tzdata` is also needed, or else UtilsTest.TimeLocal in
# src/unittest/utils_test.cc fails.
RUN apt-get -qq update && \
    apt-get -y upgrade && \
    apt-get -y install software-properties-common mg build-essential protobuf-compiler && \
    apt-get -y install python libprotobuf-dev libcurl4-openssl-dev && \
    apt-get -y install libboost-all-dev libncurses5-dev libjemalloc-dev && \
    apt-get -y install wget m4 git debhelper libssl-dev tzdata && \
    add-apt-repository -y ppa:ubuntu-toolchain-r/test && \
    apt-get update && \
    apt-get install -y gcc-5 g++-5 python-pip && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5

# Add files.
ADD . /rebirthdb

# Define working directory.
WORKDIR /rebirthdb

# Set volume.
# Build artifacts are written to /rebirthdb/build/ while Unittests write to /tmp
# Failing to set the "/tmp" volume causes the BtreeMetadata Tests to fail
# with SEGFAULT errors, read as Error Code 133 (EHWPOISON: Memory page has hardware error)
# on travis.
VOLUME [ "/rebirthdb", "/tmp" ]

# Define the default command
CMD [ "/bin/bash" ]