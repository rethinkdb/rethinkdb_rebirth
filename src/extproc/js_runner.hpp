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

#ifndef EXTPROC_JS_RUNNER_HPP_
#define EXTPROC_JS_RUNNER_HPP_

#include <string>
#include <vector>
#include <set>

#include "errors.hpp"
#include <boost/variant.hpp>

#include "containers/scoped.hpp"
#include "containers/counted.hpp"
#include "rdb_protocol/datum.hpp"
#include "concurrency/wait_any.hpp"
#include "arch/timing.hpp"
#include "extproc/extproc_job.hpp"

// Unique ids used to refer to objects on the JS side.
typedef uint64_t js_id_t;
const js_id_t INVALID_ID = 0;

// JS calls result either in a DATUM return value, a function id (which we can
// use to call the function later), or an error string
typedef boost::variant<ql::datum_t, js_id_t, std::string> js_result_t;

class extproc_pool_t;
class js_runner_t;
class js_job_t;
class js_timeout_sentry_t;

// A handle to a running "javascript evaluator" job.
class js_runner_t : public home_thread_mixin_t {
public:
    // Starts the javascript environment in process
    explicit js_runner_t(const ql::configured_limits_t &limits);
    ~js_runner_t();

    // Used for worker configuration
    struct req_config_t {
        uint64_t timeout_ms;
    };

    // Evalute JS source string to either a value or a function ID to call later
    js_result_t eval(const std::string &source,
                     const req_config_t &config);

    // Calls a previously compiled function.
    js_result_t call(const std::string &source,
                     const std::vector<ql::datum_t> &args,
                     const req_config_t &config);

private:
    static const size_t CACHE_SIZE;

    void cache_id(js_id_t id, const std::string &source);
    void trim_cache();

    // Invalidates an ID, dereferencing the object it refers to in the
    // javascript evaluator process.
    void release_id(js_id_t id);


    class job_data_t;
    scoped_ptr_t<job_data_t> job_data;

    DISABLE_COPYING(js_runner_t);
};

#endif /* EXTPROC_JS_RUNNER_HPP_ */
