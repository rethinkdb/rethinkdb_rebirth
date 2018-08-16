FROM centos:7

LABEL maintainer="RebirthDB <rebirthdb-infra@googlegroups.com>"

# Install.
# These include the build and packaging dependencies
# Make sure to install the latest git.
RUN yum install -y http://opensource.wandisco.com/centos/7/git/x86_64/wandisco-git-release-7-2.noarch.rpm && \
    yum install -y openssl-devel libcurl-devel wget curl tar m4 git \
                     boost-static m4 gcc-c++ ncurses-devel which \
                     make ncurses-static zlib-devel zlib-static \
                     epel-release protobuf-devel protobuf-static \
                     jemalloc-devel bzip2 patch && \
    yum install -y ruby-devel rpm-build rubygems && \
    gem install --no-ri --no-rdoc fpm && \
    fpm --version && \
    yum clean all

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