FROM ubuntu:16.04

# Install requisite packages
RUN apt-get -qq update && \
    apt-get -y upgrade && \
    apt-get -y install software-properties-common mg build-essential protobuf-compiler && \
    apt-get -y install python libprotobuf-dev libcurl4-openssl-dev && \
    apt-get -y install libboost-all-dev libncurses5-dev libjemalloc-dev && \
    apt-get -y install wget m4 git debhelper libssl-dev && \
    add-apt-repository -y ppa:ubuntu-toolchain-r/test && \
    apt-get update && \
    apt-get install -y gcc-5 g++-5 python-pip && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5 && \
    pip install --user cpp-coveralls

# Add files.
ADD . /rebirthdb

# Set ARGs
ARG CXX_FLAGS

# Set environment variables.
ENV CXX_FLAGS $CXX_FLAGS

# Define working directory.
WORKDIR /rebirthdb

VOLUME [ "/rebirthdb" ]

# Define the default command
CMD [ "/bin/sh" ]