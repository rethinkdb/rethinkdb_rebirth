FROM ubuntu:16.04

LABEL maintainer="RebirthDB <rebirthdb-infra@googlegroups.com>"

ENV DEBIAN_FRONTEND noninteractive

# Install requisite packages
# For xenial, `tzdata` is also needed, or else UtilsTest.TimeLocal in
# src/unittest/utils_test.cc fails.
# These include the build and packaging dependencies
RUN apt-get -qq update && \
    apt-get -y upgrade && \
    apt-get -y --no-install-recommends install software-properties-common mg build-essential protobuf-compiler && \
    apt-get -y --no-install-recommends install python libprotobuf-dev libcurl4-openssl-dev && \
    apt-get -y --no-install-recommends install libboost-all-dev libncurses5-dev libjemalloc-dev && \
    apt-get -y --no-install-recommends install wget curl m4 git debhelper fakeroot libssl-dev tzdata && \
    add-apt-repository -y ppa:ubuntu-toolchain-r/test && \
    apt-get update && \
    apt-get install -y --no-install-recommends gcc-5 g++-5 && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5 && \
    rm -rf /var/lib/apt/lists/*

# Copy files.
COPY . /rebirthdb

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