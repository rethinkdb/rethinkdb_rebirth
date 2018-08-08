#!/bin/sh

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

# This script is for generating a changelog file for the Debian source package. git-dch is problematic, so we generate a stub changelog each time.
# The script omits automatic base-finding and presently expects to be run from the root of the repository.
# Variables expected in the environment include
#   PRODUCT_NAME
#   PRODUCT_VERSION
#   OS_RELEASE
# .

TIMESTAMP_TIME="`date "+%a, %d %b %Y %H:%M:%S"`" ;
TIMESTAMP_OFFSET="-0800" ;
TIMESTAMP_FULL="$TIMESTAMP_TIME"" ""$TIMESTAMP_OFFSET" ;
AGENT_NAME="RebirthDB"
AGENT_MAIL="rebirthdb-infra@googlegroups.com"

echo "$PRODUCT_NAME ($PRODUCT_VERSION) $OS_RELEASE; urgency=low"
echo
echo "  * Release."
echo
echo " -- $AGENT_NAME <$AGENT_MAIL>  $TIMESTAMP_FULL"
# Note that there are two spaces between the e-mail address and the time-stamp. This was no accident.

