FROM ubuntu:14.04

# Install.
RUN apt-get -qq update && \
    apt-get -y upgrade && \
    apt-get -y install -y software-properties-common mg build-essential protobuf-compiler && \
    apt-get -y install python libprotobuf-dev libcurl4-openssl-dev && \
    apt-get -y install libboost-all-dev libncurses5-dev libjemalloc-dev && \
    apt-get -y install wget m4 git debhelper libssl-dev python-pip && \
    pip install --user cpp-coveralls

VOLUME ["/rebirthdb"]

# Add files.
ADD . /rebirthdb

# Set ARGs
ARG CXX_FLAGS

# Set environment variables.
ENV CXX_FLAGS $CXX_FLAGS

# Define working directory.
WORKDIR /rebirthdb

RUN cd /rebirthdb && \
    ./configure CXXFLAGS="$CXX_FLAGS" --allow-fetch && \
    make -j $(($(eval "nproc") + 1)) && make test -j $(($(eval "nproc") + 1))

# Define entry point.
ENTRYPOINT [ "/bin/bash", "-c", "./build/release/rebirthdb-unittest" ]