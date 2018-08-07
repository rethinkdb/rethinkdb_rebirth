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

#include "containers/archive/archive.hpp"
#include "extproc/extproc_pool.hpp"
#include "extproc/extproc_spawner.hpp"
#include "extproc/js_runner.hpp"
#include "rpc/serialize_macros.hpp"
#include "unittest/extproc_test.hpp"
#include "unittest/gtest.hpp"
#include "rdb_protocol/env.hpp"

SPAWNER_TEST(JSProc, DISABLED_EvalTimeout) {
    ql::configured_limits_t limits;
    js_runner_t js_runner(limits);

    const std::string loop_source = "for (var x = 0; x < 4e10; x++) {}";

    js_runner_t::req_config_t config;
    config.timeout_ms = 10;

    ASSERT_NO_THROW({
        js_result_t result = js_runner.eval(loop_source, config);
        std::string value = boost::get<std::string>(result);
        ASSERT_EQ(strprintf("JavaScript query `%s` timed out after 0.010 seconds.", loop_source.c_str()), value);
    });
}

SPAWNER_TEST(JSProc, DISABLED_CallTimeout) {
    ql::configured_limits_t limits;
    js_runner_t js_runner(limits);

    const std::string loop_source = "(function () { for (var x = 0; x < 4e10; x++) {}})";

    js_runner_t::req_config_t config;
    config.timeout_ms = 10000;

    js_result_t result = js_runner.eval(loop_source, config);

    js_id_t *any_id = boost::get<js_id_t>(&result);
    ASSERT_TRUE(any_id != nullptr);

    config.timeout_ms = 10;

    ASSERT_NO_THROW({
        result = js_runner.call(loop_source, std::vector<ql::datum_t>(), config);
        std::string value = boost::get<std::string>(result);
        ASSERT_EQ(strprintf("JavaScript query `%s` timed out after 0.010 seconds.", loop_source.c_str()), value);
    });
}

void run_datum_test(const std::string &source_code, ql::datum_t *res_out) {
    ql::configured_limits_t limits;
    js_runner_t js_runner(limits);

    js_runner_t::req_config_t config;
    config.timeout_ms = 10000;
    ASSERT_NO_THROW({
        js_result_t result = js_runner.eval(source_code, config);

        ql::datum_t *res_datum =
            boost::get<ql::datum_t>(&result);
        ASSERT_TRUE(res_datum != nullptr);
        *res_out = *res_datum;
    });
}

SPAWNER_TEST(JSProc, LiteralNumber) {
    ql::datum_t result;
    run_datum_test("9467923", &result);
    ASSERT_TRUE(result.has());
    ASSERT_TRUE(result.get_type() == ql::datum_t::R_NUM);
    ASSERT_EQ(result.as_int(), 9467923);
}

SPAWNER_TEST(JSProc, LiteralString) {
    ql::datum_t result;
    run_datum_test("\"string data\"", &result);
    ASSERT_TRUE(result.has());
    ASSERT_TRUE(result.get_type() == ql::datum_t::R_STR);
    ASSERT_EQ(result.as_str(), "string data");
}

SPAWNER_TEST(JSProc, EvalAndCall) {
    ql::configured_limits_t limits;
    js_runner_t js_runner(limits);

    const std::string source_code = "(function () { return 10337; })";

    js_runner_t::req_config_t config;
    config.timeout_ms = 10000;
    js_result_t result = js_runner.eval(source_code, config);

    // Get the id of the function out
    js_id_t *js_id = boost::get<js_id_t>(&result);
    ASSERT_TRUE(js_id != nullptr);

    // Call the function
    ASSERT_NO_THROW({
        result = js_runner.call(source_code,
                                std::vector<ql::datum_t>(),
                                config);

        // Check results
        ql::datum_t *res_datum = boost::get<ql::datum_t>(&result);
        ASSERT_TRUE(res_datum != nullptr);
        ASSERT_TRUE(res_datum->has());
        ASSERT_TRUE(res_datum->get_type() == ql::datum_t::R_NUM);
        ASSERT_EQ(res_datum->as_int(), 10337);
    });
}

SPAWNER_TEST(JSProc, BrokenFunction) {
    ql::configured_limits_t limits;
    js_runner_t js_runner(limits);

    const std::string source_code = "(function () { return 4 / 0; })";

    js_runner_t::req_config_t config;
    config.timeout_ms = 10000;
    js_result_t result = js_runner.eval(source_code, config);

    // Get the id of the function out
    js_id_t *js_id = boost::get<js_id_t>(&result);
    ASSERT_TRUE(js_id != nullptr);

    // Call the function
    ASSERT_NO_THROW({
        result = js_runner.call(source_code,
                                std::vector<ql::datum_t>(),
                                config);

        // Get the error message
        std::string *error = boost::get<std::string>(&result);
        ASSERT_TRUE(error != nullptr);
    });
}

SPAWNER_TEST(JSProc, InvalidFunction) {
    ql::configured_limits_t limits;
    js_runner_t js_runner(limits);

    const std::string source_code = "(function() {)";

    js_runner_t::req_config_t config;
    config.timeout_ms = 10000;
    ASSERT_NO_THROW({
        js_result_t result = js_runner.eval(source_code, config);

        // Get the error message
        std::string *error = boost::get<std::string>(&result);
        ASSERT_TRUE(error != nullptr);
    });
}

SPAWNER_TEST(JSProc, InfiniteRecursionFunction) {
    ql::configured_limits_t limits;
    js_runner_t js_runner(limits);

    const std::string source_code = "(function f(x) { x = x + f(x); return x; })";

    js_runner_t::req_config_t config;
    config.timeout_ms = 60000;
    js_result_t result = js_runner.eval(source_code, config);

    // Get the id of the function out
    js_id_t *js_id = boost::get<js_id_t>(&result);
    ASSERT_TRUE(js_id != nullptr);

    // Call the function
    std::vector<ql::datum_t> args;
    args.push_back(ql::datum_t(1.0));
    ASSERT_NO_THROW({
        result = js_runner.call(source_code, args, config);

        std::string *err_msg = boost::get<std::string>(&result);

        ASSERT_EQ("RangeError: callstack limit", *err_msg);
    });
}

void run_overalloc_function_test() {
    ql::configured_limits_t limits;
    js_runner_t js_runner(limits);

    const std::string source_code = "(function f() {"
                                     "  var res = \"\";"
                                     "  while (true) {"
                                     "    res = res + \"blah\";"
                                     "  }"
                                     "  return res;"
                                     "})";

    js_runner_t::req_config_t config;
    config.timeout_ms = 60000;
    js_result_t result = js_runner.eval(source_code, config);

    // Get the id of the function out
    js_id_t *js_id = boost::get<js_id_t>(&result);
    ASSERT_TRUE(js_id != nullptr);

    // Call the function
    ASSERT_THROW(js_runner.call(source_code,
                                std::vector<ql::datum_t>(),
                                config), extproc_worker_exc_t);
}

// Disabling this test because it may cause complications depending on the user's system
// ^^^ WHAT COMPLICATIONS???
TEST(JSProc, DISABLED_OverallocFunction) {
    extproc_spawner_t extproc_spawner;
    unittest::run_in_thread_pool(run_overalloc_function_test);
}

static void passthrough_test_internal(const ql::datum_t &arg) {
    guarantee(arg.has());

    ql::configured_limits_t limits;
    js_runner_t js_runner(limits);

    const std::string source_code = "(function f(arg) { return arg; })";

    js_runner_t::req_config_t config;
    config.timeout_ms = 60000;
    js_result_t result = js_runner.eval(source_code, config);

    // Get the id of the function out
    js_id_t *js_id = boost::get<js_id_t>(&result);
    ASSERT_TRUE(js_id != nullptr);

    // Call the function
    ASSERT_NO_THROW({
        js_result_t res = js_runner.call(source_code,
                                         std::vector<ql::datum_t>(1, arg),
                                         config);

        ql::datum_t *res_datum = boost::get<ql::datum_t>(&res);
        ASSERT_TRUE(res_datum != nullptr);
        ASSERT_TRUE(res_datum->has());
        ASSERT_EQ(*res_datum, arg);
    });
}

// This test will make sure that conversion of datum_t to and from duktape types works
// correctly
SPAWNER_TEST(JSProc, Passthrough) {
    ql::configured_limits_t limits;

    // Number
    passthrough_test_internal(ql::datum_t(99.9999));
    passthrough_test_internal(ql::datum_t(99.9999));

    // String
    passthrough_test_internal(ql::datum_t(""));
    passthrough_test_internal(ql::datum_t("string str"));
    passthrough_test_internal(ql::datum_t(datum_string_t()));
    passthrough_test_internal(ql::datum_t(datum_string_t("string str")));

    // Boolean
    passthrough_test_internal(ql::datum_t::boolean(true));
    passthrough_test_internal(ql::datum_t::boolean(false));

    // Array
    ql::datum_t array_datum;
    {
        std::vector<ql::datum_t> array_data;
        array_datum = ql::datum_t(std::move(array_data), limits);
        passthrough_test_internal(array_datum);

        for (size_t i = 0; i < 100; ++i) {
            array_data.push_back(
                ql::datum_t(datum_string_t(std::string(i, 'a'))));
            std::vector<ql::datum_t> copied_data(array_data);
            array_datum = ql::datum_t(std::move(copied_data), limits);
            passthrough_test_internal(array_datum);
        }
    }


    // Object
    ql::datum_t object_datum;
    {
        std::map<datum_string_t, ql::datum_t> object_data;
        object_datum = ql::datum_t(std::move(object_data));
        passthrough_test_internal(array_datum);

        for (size_t i = 0; i < 100; ++i) {
            object_data.insert(std::make_pair(datum_string_t(std::string(i, 'a')),
                                              ql::datum_t(static_cast<double>(i))));
            std::map<datum_string_t, ql::datum_t> copied_data(object_data);
            object_datum = ql::datum_t(std::move(copied_data));
            passthrough_test_internal(array_datum);
        }
    }

    // Nested structure
    ql::datum_t nested_datum;
    {
        std::vector<ql::datum_t> nested_data;
        nested_datum = ql::datum_t(std::move(nested_data), limits);
        passthrough_test_internal(nested_datum);

        nested_data.push_back(array_datum);
        std::vector<ql::datum_t> copied_data(nested_data);
        nested_datum = ql::datum_t(std::move(copied_data), limits);
        passthrough_test_internal(nested_datum);

        nested_data.push_back(object_datum);
        copied_data = nested_data;
        nested_datum = ql::datum_t(std::move(copied_data), limits);
        passthrough_test_internal(nested_datum);
    }
}
