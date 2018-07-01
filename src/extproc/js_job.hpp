// Copyright 2010-2013 RethinkDB, all rights reserved.
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
