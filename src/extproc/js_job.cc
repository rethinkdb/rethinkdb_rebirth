// Copyright 2010-2014 RethinkDB, all rights reserved.
#include "extproc/js_job.hpp"

// TODO(grandquista)
// static int rduk_exec_timeout_check(void *user_data);
//
// #define DUK_USE_EXEC_TIMEOUT_CHECK(user_data) rduk_exec_timeout_check(user_data)
// #define DUK_USE_INTERRUPT_COUNTER

#include <duktape.h>

#include <stdint.h>

#include <limits>

#include "containers/archive/boost_types.hpp"
#include "containers/archive/stl_types.hpp"
#include "extproc/extproc_job.hpp"
#include "math.hpp"
#include "rdb_protocol/pseudo_time.hpp"
#include "rdb_protocol/configured_limits.hpp"
#include "utils.hpp"

// Picked from a hat.
#define TO_JSON_RECURSION_LIMIT  500

// Returns an empty datum on error.
static ql::datum_t js_to_datum(duk_context *ctx,
                        const ql::configured_limits_t &limits,
                        std::string *err_out);

duk_context *rduk_root_ctx = nullptr;

void maybe_initialize_duk() {
    if (rduk_root_ctx == nullptr) {
        // HSI: Better fatal handler.
        rduk_root_ctx = duk_create_heap(nullptr, nullptr, nullptr, nullptr,
                                        nullptr);
        guarantee(rduk_root_ctx != nullptr);
    }
}

static js_id_t remember_value(rduk_env_t *env, duk_context *ctx);
static js_result_t rduk_eval(rduk_env_t *env, const std::string &source,
                      const ql::configured_limits_t &limits);
static js_result_t rduk_call(js_id_t id, const std::vector<ql::datum_t> &args,
                      const ql::configured_limits_t &limits);
static void rduk_release(js_id_t id);

static js_result_t run_eval(
        std::string source,
        ql::configured_limits_t limits,
        rduk_env_t *duk_env);
static js_result_t run_call(
        js_id_t id,
        std::vector<ql::datum_t> args,
        ql::configured_limits_t limits);
static void run_release(js_id_t id);
static void run_exit();

static void worker_fn();

// The job_t runs in the context of the main rethinkdb process
js_job_t::js_job_t(const ql::configured_limits_t &_limits) :
    limits(_limits) { }

js_result_t js_job_t::eval(const std::string &source) {
    return run_eval(source, limits, &duk_env);
}

js_result_t js_job_t::call(js_id_t id, const std::vector<ql::datum_t> &args) {
    return run_call(id, args, limits);
}

void js_job_t::release(js_id_t id) {
    run_release(id);
}

void js_job_t::exit() {
    run_exit();
}

void js_job_t::worker_error() {
}

static js_result_t run_eval(
        std::string source,
        ql::configured_limits_t limits,
        rduk_env_t *duk_env) {
    worker_fn();

    js_result_t js_result;
    try {
        js_result = rduk_eval(duk_env, source, limits);
    } catch (const std::exception &e) {
        js_result = e.what();
    } catch (...) {
        js_result = std::string("encountered an unknown exception");
    }

    return js_result;
}

static js_result_t run_call(
        js_id_t id,
        std::vector<ql::datum_t> args,
        ql::configured_limits_t limits) {
    worker_fn();

    js_result_t js_result;
    try {
        js_result = rduk_call(id, args, limits);
    } catch (const std::exception &e) {
        js_result = e.what();
    } catch (...) {
        js_result = std::string("encountered an unknown exception");
    }

    return js_result;
}

static void run_release(js_id_t id) {
    worker_fn();

    rduk_release(id);
}

static void run_exit() {
    worker_fn();
}

static void worker_fn() {
    static uint64_t task_counter = 0;
    maybe_initialize_duk();

    task_counter += 1;

    if (task_counter % 128 == 0) {
        // Have to call twice, per documentation, for objects with
        // finalizers.  (Are there any?)
        duk_gc(rduk_root_ctx, DUK_GC_COMPACT);
        duk_gc(rduk_root_ctx, DUK_GC_COMPACT);
    }
}

struct rduk_pop_exit {
    explicit rduk_pop_exit(duk_context *ctx, duk_idx_t count = 1) : ctx_(ctx), count_(count) {}

    void adjust(duk_idx_t adjustment) {
        count_ += adjustment;
    }

    ~rduk_pop_exit() {
        duk_pop_n(ctx_, count_);
    }
  private:
    duk_context *ctx_;
    duk_idx_t count_;

    DISABLE_COPYING(rduk_pop_exit);
};



static js_result_t rduk_eval(rduk_env_t *env, const std::string &source,
                      const ql::configured_limits_t &limits) {
    duk_require_stack(rduk_root_ctx, 1);
    duk_push_thread_new_globalenv(rduk_root_ctx);
    rduk_pop_exit pop_ctx(rduk_root_ctx);

    duk_context *ctx = duk_get_context(rduk_root_ctx, -1);
    js_result_t result("");
    static_assert(std::is_same<size_t, duk_size_t>::value, "size_t != duk_size_t");

    duk_require_stack(ctx, 1);
    duk_int_t res = duk_peval_lstring(ctx, source.data(), source.size());
    rduk_pop_exit pop_peval_result(ctx);
    if (0 != res) {
        size_t msg_size;
        const char *msg = duk_safe_to_lstring(ctx, -1, &msg_size);
        boost::get<std::string>(result).append(msg, msg_size);
    } else {
        if (duk_is_function(ctx, -1)) {
            result = remember_value(env, ctx);
        } else {
            ql::datum_t datum = js_to_datum(ctx, limits,
                                            &boost::get<std::string>(result));
            if (datum.has()) {
                result = std::move(datum);
            }
        }
    }

    return result;
}

std::string id_prop_name(js_id_t id) {
    return std::to_string(id);
}

// With ctx's stack: [...] [value] -> [...] [value]
// Puts value into the duk heap stash with property value js_id_t.
static js_id_t remember_value(rduk_env_t *env, duk_context *ctx) {
    duk_require_stack(ctx, 2);
    duk_push_heap_stash(ctx);
    rduk_pop_exit pop_stash(ctx);

    duk_dup(ctx, -2);
    js_id_t id = env->next_id;
    ++env->next_id;
    // Convert id to string, because JS indices are 32-bit, and we
    // don't reuse id's.
    std::string id_string = id_prop_name(id);
    duk_bool_t res = duk_put_prop_lstring(ctx, -2, id_string.data(), id_string.size());
    guarantee(res);
    return id;
}

static void push_find_value(duk_context *ctx, js_id_t id) {
    duk_require_stack(ctx, 2);
    duk_push_heap_stash(ctx);
    rduk_pop_exit pop_stash(ctx);

    std::string id_string = id_prop_name(id);
    static_assert(std::is_same<size_t, duk_size_t>::value, "size_t != duk_size_t");
    bool res = duk_get_prop_lstring(ctx, -1, id_string.data(), id_string.size());
    guarantee(res, "js_id_t not found");

    duk_remove(ctx, -2);
    pop_stash.adjust(-1);
}

static bool push_js_from_datum(duk_context *ctx,
                        const ql::datum_t &datum, std::string *err_out) {
    guarantee(datum.has());

    switch (datum.get_type()) {
    case ql::datum_t::type_t::MINVAL:
        err_out->assign("`r.minval` cannot be passed to `r.js`.");
        return false;
    case ql::datum_t::type_t::MAXVAL:
        err_out->assign("`r.maxval` cannot be passed to `r.js`.");
        return false;
    case ql::datum_t::type_t::R_BINARY:
        // TODO: We could support this!  With an ArrayBuffer API?
        err_out->assign("`r.binary` data cannot be used in `r.js`.");
        return false;
    case ql::datum_t::type_t::R_BOOL:
        duk_push_boolean(ctx, datum.as_bool());
        return true;
    case ql::datum_t::type_t::R_NULL:
        duk_push_null(ctx);
        return true;
    case ql::datum_t::type_t::R_NUM:
        static_assert(std::is_same<double, duk_double_t>::value, "double != duk_double_t");
        duk_push_number(ctx, datum.as_num());
        return true;
    case ql::datum_t::type_t::R_STR: {
        const datum_string_t &str = datum.as_str();
        duk_push_lstring(ctx, str.data(), str.size());
    } return true;
    case ql::datum_t::type_t::R_ARRAY: {
        duk_push_array(ctx);
        for (size_t i = 0, n = datum.arr_size(); i < n; ++i) {
            if (!push_js_from_datum(ctx, datum.get(i), err_out)) {
                duk_pop(ctx);
                return false;
            }
            // HSI: size_t->duk_idx_t truncation?
            duk_put_prop_index(ctx, -2, i);
        }
        return true;
    }
    case ql::datum_t::type_t::R_OBJECT: {
        if (datum.is_ptype(ql::pseudo::time_string)) {
            // If the user redefined Date... too bad?
            // HSI: We could make a separate globalenv and grab a true Date from there.
            double epoch_time = ql::pseudo::time_to_epoch_time(datum);

            if (!duk_get_global_lstring(ctx, "Date", 4)) {
                *err_out = "JS environment does not contain Date";
                return false;
            }

            duk_push_number(ctx, epoch_time * 1000);
            duk_new(ctx, 1);
            return true;
        } else {
            duk_push_bare_object(ctx);
            for (size_t i = 0, n = datum.obj_size(); i < n; ++i) {
                auto pair = datum.get_pair(i);
                if (!push_js_from_datum(ctx, pair.second, err_out)) {
                    duk_pop(ctx);
                    return false;
                }
                duk_put_prop_lstring(ctx, -2, pair.first.data(), pair.first.size());
            }
            return true;
        }
    }
    case ql::datum_t::type_t::UNINITIALIZED: // fallthru
    default:
        err_out->assign("bad datum value in js extproc");
        return false;
    }
}

static js_result_t rduk_call(js_id_t id, const std::vector<ql::datum_t> &args,
                      const ql::configured_limits_t &limits) {
    duk_require_stack(rduk_root_ctx, 1);
    duk_push_thread_new_globalenv(rduk_root_ctx);
    rduk_pop_exit pop_root_ctx(rduk_root_ctx);

    duk_context *ctx = duk_get_context(rduk_root_ctx, -1);

    duk_idx_t num_args = args.size();
    duk_require_stack(ctx, 1 + num_args);
    // Alright, we pushed the function onto the stack.
    push_find_value(ctx, id);
    rduk_pop_exit popper(ctx, 1);

    js_result_t result("");

    for (const ql::datum_t &datum : args) {
        if (!push_js_from_datum(ctx, datum, &boost::get<std::string>(result))) {
            return result;
        }
        popper.adjust(1);
    }

    guarantee(args.size() <= std::numeric_limits<duk_idx_t>::max());
    // stack: [...] [fn] [arg1] [arg2] ... [argN]
    bool success = (0 == duk_pcall(ctx, num_args));
    popper.adjust(-num_args);
    if (!success) {
        // stack: [...] [err]
        size_t str_len;
        const char *str = duk_safe_to_lstring(ctx, -1, &str_len);
        boost::get<std::string>(result).append(str, str_len);
        return result;
    }
    // stack: [...] [retval]

    if (duk_is_function(ctx, -1)) {
        boost::get<std::string>(result) = "Returning functions from within `r.js` is unsupported.";
        return result;
    }

    ql::datum_t datum = js_to_datum(ctx, limits, &boost::get<std::string>(result));
    if (datum.has()) {
        result = std::move(datum);
    }

    return result;
}

static void rduk_release(js_id_t id) {
    duk_push_heap_stash(rduk_root_ctx);
    rduk_pop_exit pop_stash(rduk_root_ctx);

    std::string id_string = id_prop_name(id);
    static_assert(std::is_same<size_t, duk_size_t>::value, "size_t != duk_size_t");
    bool res = duk_del_prop_lstring(rduk_root_ctx, -1, id_string.data(),
                                    id_string.size());
    guarantee(res, "released non-present js val");
}

static bool rduk_is_of_type(duk_context *ctx, const char *typ) {
    duk_require_stack(ctx, 3);
    // stack: [...] [value]
    bool got_global = duk_get_global_string(ctx, typ);
    // stack: [...] [value] [Date]
    guarantee(got_global);
    bool got_prop = duk_get_prop_string(ctx, -1, "prototype");
    // stack: [...] [value] [Date] [Date.prototype]
    guarantee(got_prop);
    // stack: [...] [value] [Date] [Date.prototype] [value's prototype]
    duk_get_prototype(ctx, -3);
    bool equal = duk_strict_equals(ctx, -1, -2);
    duk_pop_3(ctx);
    return equal;
}

// TODO: Is there a better way of detecting circular references than a recursion limit?
// With ctx's stack: [...] [value] -> [...] [value]
// Constructs a datum from value.
static ql::datum_t js_make_datum(duk_context *ctx,
                          int recursion_limit,
                          const ql::configured_limits_t &limits,
                          std::string *err_out) {
    ql::datum_t result;

    if (0 == recursion_limit) {
        err_out->assign("Recursion limit exceeded in js_to_json (circular reference?).");
        return result;
    }
    --recursion_limit;

    static_assert(std::is_same<size_t, duk_size_t>::value, "size_t != duk_size_t");
    size_t string_byte_len;
    if (const char *string = duk_get_lstring(ctx, -1, &string_byte_len)) {
        // TODO: I have no idea why this try/catch is needed.
        try {
            result = ql::datum_t(datum_string_t(string_byte_len, string));
        } catch (const ql::base_exc_t &ex) {
            err_out->assign(ex.what());
        }
    } else if (duk_is_object(ctx, -1)) {
        if (duk_is_array(ctx, -1)) {
            duk_require_stack(ctx, 1);
            std::vector<ql::datum_t> datum_array;
            size_t n = duk_get_length(ctx, -1);
            datum_array.reserve(n);
            for (size_t i = 0; i < n; ++i) {
                duk_get_prop_index(ctx, -1, i);
                rduk_pop_exit pop_elem(ctx);

                ql::datum_t item = js_make_datum(ctx, recursion_limit, limits, err_out);
                if (!item.has()) {
                    // Result is still empty, the error message has been set
                    return result;
                }
                datum_array.push_back(std::move(item));
            }

            result = ql::datum_t(std::move(datum_array), limits);
        } else if (duk_is_function(ctx, -1)) {
            err_out->assign("Cannot convert function to ql::datum_t.");
        } else if (rduk_is_of_type(ctx, "RegExp")) {
            // We can't represent regular expressions in datums
            err_out->assign("Cannot convert RegExp to ql::datum_t.");
        } else if (rduk_is_of_type(ctx, "Date")) {
            duk_require_stack(ctx, 1);
            // Okay, now call valueOf on ctx.
            duk_push_string(ctx, "valueOf");
            rduk_pop_exit popper(ctx);
            // stack: [...] [date_value] ["valueOf"]
            bool res = duk_pcall_prop(ctx, -1, 0);
            // stack: [...] [date_value] [retval]
            if (res != 0) {
                err_out->assign("Invalid Date in JS environment");
            } else if (!duk_is_number(ctx, -1)) {
                err_out->assign("Invalid Date valueOf in JS environment");
            } else {
                double num_val = duk_get_number(ctx, -1);
                result = ql::pseudo::make_time(num_val * (1.0 / 1000.0), "+00:00");
            }
        } else {
            // Treat it as a dictionary.

            // We'll push enumerator, key, value here.
            duk_require_stack(ctx, 3);

            ql::datum_object_builder_t builder;

            duk_enum(ctx, -1, 0);
            rduk_pop_exit pop_enum(ctx);

            // stack is [...] [value] [enumerator]
            while (duk_next(ctx, -1, 1 /* get the value */)) {
                rduk_pop_exit pop_key(ctx);
                rduk_pop_exit pop_value(ctx);

                // stack is [...] [value] [enumerator] [key] [value]
                ql::datum_t item = js_make_datum(ctx, recursion_limit, limits, err_out);

                if (!item.has()) {
                    // Result is still empty, the error message has been set
                    return result;
                }

                size_t key_byte_len;
                const char *key = duk_get_lstring(ctx, -2, &key_byte_len);
                datum_string_t key_string(key_byte_len, key);
                builder.overwrite(key_string, std::move(item));
            }

            result = std::move(builder).to_datum();
        }
    } else if (duk_is_number(ctx, -1)) {
        double num_val = duk_get_number(ctx, -1);
        if (!risfinite(num_val)) {
            err_out->assign("Number return value is not finite.");
        } else {
            result = ql::datum_t(num_val);
        }
    } else if (duk_is_boolean(ctx, -1)) {
        result = ql::datum_t::boolean(duk_get_boolean(ctx, -1));
    } else if (duk_is_null(ctx, -1)) {
        err_out->assign(duk_is_undefined(ctx, -1) ?
                       "Cannot convert javascript `undefined` to ql::datum_t." :
                       "Unrecognized value type when converting to ql::datum_t.");
    }

    return result;
}

static ql::datum_t js_to_datum(duk_context *ctx,
                        const ql::configured_limits_t &limits,
                        std::string *err_out) {
    return js_make_datum(ctx, TO_JSON_RECURSION_LIMIT, limits, err_out);
}
