/* Copyright (c) 2020 github.com/univrsal <universailp@web.de>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <assert.h>
#include <cmath>
#include <csignal.h>
#include <cstdio>
#include <iostream>
#include <signal.hpp>

#define FLOAT_LENIENCY 0.00001

typedef struct point_s {
    int x, y;
} point_t;

using namespace std;

template <class T>
void test_param(const signal::parameters& in, const char* id, const T& def, const T& expect, bool expect2)
{
    bool ok = false;
    const auto& val = in.get<T>(id, &ok, def);
    cout << "Start test with " << id << " for " << val << " against "
         << expect << " could retrieve: " << ok << ", expect: " << expect2 << endl;

    assert(val == expect);
    assert(ok == expect2);
}

template <class T>
void test_paramf(const signal::parameters& in, const char* id, const T& def, const T& expect, bool expect2)
{
    bool ok = false;
    const auto& val = in.get<T>(id, &ok, def);
    if (expect2)
        assert(fabs(static_cast<double>(val - expect)) < FLOAT_LENIENCY);
    assert(ok == expect2);
}

void cpp_signal1(const signal::parameters& in, signal::parameters*)
{
    cout << "== Triggered C++ signal 1" << endl;

    test_param<int>(in, "int", 0, -255, true);
    test_param<int>(in, "int2", -1, -1, false);
    test_param<unsigned int>(in, "uint", 0, 255, true);
    test_param<unsigned int>(in, "uint2", 1, 1, false);
    test_paramf<float>(in, "float", 0, 3.14f, true);
    test_paramf<float>(in, "float2", -3.14f, 3.14f, false);
    test_paramf<double>(in, "double", 0, 3.141, true);
    test_paramf<double>(in, "double2", -3.14, 3.141, false);
    test_param<bool>(in, "bool", false, true, true);
    test_param<bool>(in, "bool2", false, false, false);
    test_param<string>(in, "string", string(""), string("test123"), true);
    test_param<string>(in, "string2", string("doesn't exist"), string("doesn't exist"), false);
}

void cpp_signal1_2(const signal::parameters&, signal::parameters* out)
{
    cout << "== Triggered C++ signal 1 take 2" << endl;
    if (out) {
        assert(out->add<int>("int", -255));
        assert(out->add<unsigned int>("uint", 255));
        assert(out->add<float>("float", 3.14f));
        assert(out->add<double>("double", 3.141));
        assert(out->add<bool>("bool", true));
        assert(out->add<string>("string", string("test123")));
    }
}

void cpp_signal2(const signal::parameters&, signal::parameters*)
{
    cout << "== Triggered C++ signal 2" << endl;
}

void register_cpp_signals(signal::manager& man)
{
    assert(man.add("signal1", cpp_signal1));
    assert(man.add("signal1", cpp_signal1_2));
    assert(man.add("signal2", cpp_signal2));
}

void c_signal1(const signal_parameters_t* in, signal_parameters_t*)
{
    printf("== Triggered C signal 1\n");
    bool ok = false;

    int val = signal_parameters_get_int(in, "int", &ok);
    assert(val == -255 && ok);

    signal_parameters_get_int(in, "int2", &ok);
    assert(!ok);

    unsigned int val2 = signal_parameters_get_uint(in, "uint", &ok);
    assert(val2 == 255 && ok);

    signal_parameters_get_uint(in, "uint2", &ok);
    assert(!ok);

    float val3 = signal_parameters_get_float(in, "float", &ok);
    assert(fabs(static_cast<double>(val3 - 3.14f)) < FLOAT_LENIENCY && ok);

    signal_parameters_get_float(in, "float2", &ok);
    assert(!ok);

    double val4 = signal_parameters_get_double(in, "double", &ok);
    assert(fabs(static_cast<double>(val4 - 3.141)) < FLOAT_LENIENCY && ok);

    bool val5 = signal_parameters_get_bool(in, "bool", &ok);
    assert(val5 && ok);

    signal_parameters_get_bool(in, "bool2", &ok);
    assert(!ok);

    const char* val6 = signal_parameters_get_string(in, "string", &ok);
    assert(strcmp(val6, "string123") == 0 && ok);

    signal_parameters_get_string(in, "string2", &ok);
    assert(!ok);

    const point_t* val7 = reinterpret_cast<const point_t*>(signal_parameters_get_data(in, "data", &ok));
    assert(val7->x == 2 && val7->y == 3 && ok);

    signal_parameters_get_data(in, "data2", &ok);
    assert(!ok);
}

void c_signal1_2(const signal_parameters_t*, signal_parameters_t* out)
{
    printf("== Triggered C signal 1_2\n");
    if (out) {
        assert(signal_parameters_set_int(out, "int", -255));
        assert(signal_parameters_set_uint(out, "uint", 255));
        assert(signal_parameters_set_float(out, "float", 3.14f));
        assert(signal_parameters_set_double(out, "double", 3.141));
        assert(signal_parameters_set_bool(out, "bool", true));
        assert(signal_parameters_set_string(out, "string", "string123"));

        point_t p;
        p.x = 2;
        p.y = 3;
        assert(signal_parameters_set_data(out, "data", &p, sizeof(point_t)));
    }
}

void c_signal2(const signal_parameters_t*, signal_parameters_t*)
{
    printf("== Triggered C signal 2\n");
}

void register_c_signals(signal_manager_t* man)
{
    assert(signal_add(man, "signal1", c_signal1));
    assert(signal_add(man, "signal1_2", c_signal1_2));
    assert(signal_add(man, "signal2", c_signal2));
}

int signal_cpp_test()
{
    cout << "---- C++ Test ----" << endl;

    signal::manager m;
    register_cpp_signals(m);
    signal::parameters out, in;

    cout << "--- Setting up input paramters ---" << endl;
    assert(in.add<int>("int", -255));
    assert(in.add<unsigned int>("uint", 255));
    assert(in.add<float>("float", 3.14f));
    assert(in.add<double>("double", 3.141));
    assert(in.add<bool>("bool", true));
    assert(in.add<string>("string", string("test123")));

    cout << "--- Firing Signals ---" << endl;
    assert(m.send("signal1", in, &out));
    assert(m.send("signal2"));

    cout << "--- Checking output parameters ---" << endl;
    test_param<int>(out, "int", 0, -255, true);
    test_param<unsigned int>(out, "uint", 0, 255, true);
    test_paramf<float>(out, "float", 0, 3.14f, true);
    test_paramf<double>(out, "double", 0, 3.141, true);
    test_param<bool>(out, "bool", false, true, true);
    test_param<string>(out, "string", string(""), string("test123"), true);
    return 0;
}

int signal_c_test()
{
    printf("---- C Test ----\n");

    signal_manager_t* m = signal_manager_create();
    signal_parameters_t* in = signal_parameters_create();
    signal_parameters_t* out = signal_parameters_create();
    register_c_signals(m);

    printf("--- Setting up input paramters ---\n");
    assert(signal_parameters_set_int(in, "int", -255));
    assert(signal_parameters_set_uint(in, "uint", 255));
    assert(signal_parameters_set_float(in, "float", 3.14f));
    assert(signal_parameters_set_double(in, "double", 3.141));
    assert(signal_parameters_set_bool(in, "bool", true));
    assert(signal_parameters_set_string(in, "string", "string123"));

    point_t p;
    p.x = 2;
    p.y = 3;
    assert(signal_parameters_set_data(in, "data", &p, sizeof(point_t)));

    printf("--- Firing signals ---\n");
    assert(signal_send(m, "signal1", in, out));
    assert(signal_send(m, "signal2", nullptr, nullptr));

    printf("--- Checking output paramters ---\n");
    bool ok = false;
    int val = signal_parameters_get_int(in, "int", &ok);
    assert(val == -255 && ok);
    unsigned int val2 = signal_parameters_get_uint(in, "uint", &ok);
    assert(val2 == 255 && ok);
    float val3 = signal_parameters_get_float(in, "float", &ok);
    assert(fabs(static_cast<double>(val3 - 3.14f)) < FLOAT_LENIENCY && ok);
    double val4 = signal_parameters_get_double(in, "double", &ok);
    assert(fabs(static_cast<double>(val4 - 3.141)) < FLOAT_LENIENCY && ok);
    const char* val6 = signal_parameters_get_string(in, "string", &ok);
    assert(strcmp(val6, "string123") == 0 && ok);
    const point_t* val7 = reinterpret_cast<const point_t*>(signal_parameters_get_data(in, "data", &ok));
    assert(val7->x == 2 && val7->y == 3 && ok);

    printf("--- Freeing memory\n");
    signal_parameters_free(in);
    signal_parameters_free(out);
    signal_manager_free(m);
    return 0;
}
