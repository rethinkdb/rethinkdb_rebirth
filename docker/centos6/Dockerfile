FROM centos:6

LABEL maintainer="RebirthDB <rebirthdb-infra@googlegroups.com>"

# Install.
# These include the build and packaging dependencies
RUN yum install -y openssl-devel libcurl-devel wget tar m4 git-core \
                     boost-static m4 gcc-c++ npm ncurses-devel which \
                     make ncurses-static zlib-devel zlib-static \
                     epel-release protobuf-devel protobuf-static \
                     jemalloc-devel bzip2 patch && \
    yum install gnupg2 && yum clean all \
    gpg2 --list-keys &&
    gpg2 --recv-keys 409B6B1796C275462A1703113804BB82D39DC0E3 && \
    curl -L get.rvm.io | bash -s stable && \
    source /home/root/.rvm/scripts/rvm && \
    rvm reload && rvm install ruby --latest && \
    gem install --no-ri --no-rdoc fpm && \
    fpm --version

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