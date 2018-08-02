#!/bin/bash

# Copyright 2018-present RebirthDB
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not use
# this file except in compliance with the License. You may obtain a copy of the
# License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software distributed
# under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#
# This file incorporates work covered by the following copyright:
#
#     Copyright 2010-present, The Linux Foundation, portions copyright Google and
#     others and used with permission or subject to their respective license
#     agreements.
#
#     Licensed under the Apache License, Version 2.0 (the "License");
#     you may not use this file except in compliance with the License.
#     You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#     Unless required by applicable law or agreed to in writing, software
#     distributed under the License is distributed on an "AS IS" BASIS,
#     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#     See the License for the specific language governing permissions and
#     limitations under the License.

# This script is used to build RebirthDB RPM on CentOS 6.4 and 7
#
# It requires:
# * The build dependencies: https://www.rethinkdb.com/docs/install/centos/
# * gem install fpm

set -eu

main () {
    MAKEFLAGS=${MAKEFLAGS:- -j 8}
    export MAKEFLAGS
    ARCH=`gcc -dumpmachine | cut -f 1 -d -`
    RPM_ROOT=build/packages/rpm
    VERSION=`./scripts/gen-version.sh | sed -e s/-/_/g`
    RPM_PACKAGE=build/packages/rebirthdb-$VERSION.$ARCH.rpm
    SYMBOLS_FILE_IN=build/release/rebirthdb.debug
    SYMBOLS_FILE_OUT=$RPM_PACKAGE.debug.bz2
    DESCRIPTION='RebirthDB is built to store JSON documents, and scale to multiple servers with very little effort. It has a pleasant query language that supports really useful queries like table joins and group by.'
    tmpfile BEFORE_INSTALL <<EOF
getent group rebirthdb >/dev/null || groupadd -r rebirthdb
getent passwd rebirthdb >/dev/null || \
    useradd --system --no-create-home --gid rebirthdb --shell /sbin/nologin \
    --comment "RebirthDB Daemon" rebirthdb
EOF

    test -n "${NOCONFIGURE:-}" || ./configure --allow-fetch --prefix=/usr --sysconfdir=/etc --localstatedir=/var

    `make command-line` install DESTDIR=$RPM_ROOT BUILD_PORTABLE=1 SPLIT_SYMBOLS=1

    ... () { command="$command $(for x in "$@"; do printf "%q " "$x"; done)"; }

    GLIBC_VERSION=`rpm -qa --queryformat '%{VERSION}\n' glibc | head -n1`

    command=fpm
    ... -t rpm                  # Build an RPM package
    ... --package $RPM_PACKAGE
    ... --name rebirthdb
    ... --license 'ASL 2.0'
    ... --vendor RebirthDB
    ... --category Database
    ... --version "$VERSION"
    ... --iteration "`./scripts/gen-version.sh -r`"
    ... --depends "glibc >= $GLIBC_VERSION"
    ... --conflicts 'rebirthdb'
    ... --architecture "$ARCH"
    ... --maintainer 'RebirthDB <rebirthdb-infra@googlegroups.com>'
    ... --description "$DESCRIPTION"
    ... --url 'http://www.rethinkdb.com/'
    ... --before-install "$BEFORE_INSTALL"
    ... -s dir -C $RPM_ROOT     # Directory containing the installed files
    ... usr etc var             # Directories to package in the package
    eval $command

    bzip2 -c "$SYMBOLS_FILE_IN" > "$SYMBOLS_FILE_OUT"
}

tmpfile () {
    local _file=`mktemp`
    cat >"$_file"
    at_exit rm "$_file"
    eval "$1=$(printf %q "$_file")"
}

at_exit () {
    local cmd=
    for x in "$@"; do
        cmd="$cmd $(printf %q "$x")"
    done
    AT_EXIT_ALL=${AT_EXIT_ALL:-}'
'"$cmd"
    trap exit_handler EXIT
}

exit_handler () {
    eval "$AT_EXIT_ALL"
}

main "$@"
