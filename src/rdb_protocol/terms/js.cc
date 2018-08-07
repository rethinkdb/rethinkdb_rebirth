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

#include <stdint.h>

#include <string>

#include "rdb_protocol/error.hpp"
#include "rdb_protocol/func.hpp"
#include "rdb_protocol/op.hpp"
#include "rdb_protocol/terms/terms.hpp"
#include "extproc/js_runner.hpp"

namespace ql {

class javascript_term_t : public op_term_t {
public:
    javascript_term_t(compile_env_t *env, const raw_term_t &term)
        : op_term_t(env, term, argspec_t(1), optargspec_t({ "timeout" })) { }

private:
    virtual scoped_ptr_t<val_t> eval_impl(scope_env_t *env,
                                          args_t *args,
                                          eval_flags_t) const {
        // Optarg seems designed to take a default value as the second argument
        // but nowhere else is this actually used.
        uint64_t timeout_ms = 5000;
        scoped_ptr_t<val_t> timeout_opt = args->optarg(env, "timeout");
        if (timeout_opt) {
            if (timeout_opt->as_num() > static_cast<double>(UINT64_MAX) / 1000) {
                timeout_ms = UINT64_MAX;
            } else {
                timeout_ms = timeout_opt->as_num() * 1000;
            }
        }

        std::string source = args->arg(env, 0)->as_datum().as_str().to_std();

        // JS runner configuration is limited to setting an execution timeout.
        js_runner_t::req_config_t config;
        config.timeout_ms = timeout_ms;

        try {
            js_result_t result = js_runner_t(env->env->limits()).eval(source, config);
            return scoped_ptr_t<val_t>(
                    boost::apply_visitor(js_result_visitor_t(source, timeout_ms, this),
                                         result));
        } catch (const extproc_worker_exc_t &e) {
            rfail(base_exc_t::INTERNAL,
                  "Javascript query `%s` caused a crash in a worker process.",
                  source.c_str());
        }
    }
    virtual const char *name() const { return "javascript"; }

    // No JS term is considered deterministic
    virtual deterministic_t is_deterministic() const {
        return deterministic_t::no();
    }
};

counted_t<term_t> make_javascript_term(
        compile_env_t *env, const raw_term_t &term) {
    return make_counted<javascript_term_t>(env, term);
}

}  // namespace ql
