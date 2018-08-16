FROM ubuntu:18.04

LABEL maintainer="RebirthDB <rebirthdb-infra@googlegroups.com>"

ENV DEBIAN_FRONTEND noninteractive

# Install requisite packages
# These include the build and packaging dependencies
RUN apt-get -qq update && \
    apt-get -y upgrade && \
    apt-get -y --no-install-recommends install software-properties-common mg build-essential protobuf-compiler && \
    apt-get -y --no-install-recommends install python libprotobuf-dev libcurl4-openssl-dev && \
    apt-get -y --no-install-recommends install libboost-all-dev libncurses5-dev libjemalloc-dev && \
    apt-get -y --no-install-recommends install wget curl m4 git debhelper fakeroot libssl-dev tzdata && \
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