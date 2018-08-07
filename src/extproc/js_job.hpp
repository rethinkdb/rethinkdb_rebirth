// Copyright 2018-present RebirthDB
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
// This file incorporates work covered by the following copyright:
//
//     Copyright 2010-present, The Linux Foundation, portions copyright Google and
//     others and used with permission or subject to their respective license
//     agreements.
//
//     Licensed under the Apache License, Version 2.0 (the "License");
//     you may not use this file except in compliance with the License.
//     You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//     Unless required by applicable law or agreed to in writing, software
//     distributed under the License is distributed on an "AS IS" BASIS,
//     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//     See the License for the specific language governing permissions and
//     limitations under the License.

#ifndef EXTPROC_JS_JOB_HPP_
#define EXTPROC_JS_JOB_HPP_

#include <string>
#include <vector>

#include "utils.hpp"
#include "containers/archive/archive.hpp"
#include "containers/counted.hpp"
#include "concurrency/signal.hpp"
#include "extproc/extproc_pool.hpp"
#include "extproc/extproc_job.hpp"
#include "extproc/js_runner.hpp"
#include "rdb_protocol/datum.hpp"

const js_id_t MIN_ID = 1;

struct rduk_env_t {
    js_id_t next_id = MIN_ID;
    // The duk heap's stash map is used to access persistent values by
    // js_id_t.  (That is duk's tool for keeping persistent values
    // reachable by GC.)
};

class js_job_t {
public:
    explicit js_job_t(
        const ql::configured_limits_t &limits);

    js_result_t eval(const std::string &source);
    js_result_t call(js_id_t id, const std::vector<ql::datum_t> &args);
    void release(js_id_t id);
    void exit();

    // Marks the extproc worker as errored to simplify cleanup later
    void worker_error();

private:

    rduk_env_t duk_env;
    ql::configured_limits_t limits;
    DISABLE_COPYING(js_job_t);
};

#endif /* EXTPROC_JS_JOB_HPP_ */
