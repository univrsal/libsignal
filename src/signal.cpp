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

#include "libsignal.h"
#include <string.h>

typedef struct signal_manager_s {
    signal::manager man;
} signal_manager_t;

typedef struct signal_parameters_s {
    signal::parameters param;
} signal_parameters_t;

signal_manager_t *signal_manager_create(void)
{
    auto *m = new signal_manager_t;
    m->man = signal::manager();
    return m;
}

void signal_manager_free(signal_manager_t *m)
{
    delete m;
}

signal_parameters_t *signal_parameters_create(void)
{
    auto *sp = new signal_parameters_t;
    return sp;
}

void signal_parameters_free(signal_parameters_t *p)
{
    delete p;
}

bool signal_send(signal_manager_t *m, const char *id,
                 const signal_parameters_t *param, signal_parameters_t *out)
{
    if (!m || !id)
        return false;
    return m->man.send(id, param->param, &out->param);
}

bool signal_add(signal_manager_t *m, const char *id, signal_function_t fun)
{
    if (!m || !fun || !id)
        return false;
    return m->man.add(id, reinterpret_cast<signal::signal_function>(fun));
}

bool signal_parameters_set_int(signal_parameters_t *p, const char *id, int val)
{
    if (!p || !id)
        return false;
    return p->param.add<int>(id, val);
}

bool signal_parameters_set_uint(signal_parameters_t *p, const char *id,
                                unsigned int val)
{
    if (!p || !id)
        return false;
    return p->param.add<unsigned int>(id, val);
}

bool signal_parameters_set_bool(signal_parameters_t *p, const char *id,
                                bool val)
{
    if (!p || !id)
        return false;
    return p->param.add<bool>(id, val);
}

bool signal_parameters_set_float(signal_parameters_t *p, const char *id,
                                 float val)
{
    if (!p || !id)
        return false;
    return p->param.add<float>(id, val);
}

bool signal_parameters_set_double(signal_parameters_t *p, const char *id,
                                  double val)
{
    if (!p || !id)
        return false;
    return p->param.add<double>(id, val);
}

bool signal_parameters_set_string(signal_parameters_t *p, const char *id,
                                  const char *val)
{
    if (!p || !id)
        return false;
    return p->param.add<std::string>(id, std::string(val));
}

bool signal_parameters_set_data(signal_parameters_t *p, const char *id,
                                void *val, size_t size)
{
    if (!p || !id || !val)
        return false;
    void *d = malloc(size);
    memcpy(d, &val, size);
    return p->param.add_direct(id, val, size);
}

int signal_parameters_get_int(const signal_parameters_t *p, const char *id,
                              bool *ok)
{
    if (!p || !id) {
        if (ok)
            *ok = false;
        return 0;
    }
    return p->param.get<int>(id, ok);
}

unsigned int signal_parameters_get_uint(const signal_parameters_t *p,
                                        const char *id, bool *ok)
{
    if (!p || !id) {
        if (ok)
            *ok = false;
        return 0;
    }
    return p->param.get<unsigned int>(id, ok);
}

bool signal_parameters_get_bool(const signal_parameters_t *p, const char *id,
                                bool *ok)
{
    if (!p || !id) {
        if (ok)
            *ok = false;
        return 0;
    }
    return p->param.get<bool>(id, ok);
}

float signal_parameters_get_float(const signal_parameters_t *p, const char *id,
                                  bool *ok)
{
    if (!p || !id) {
        if (ok)
            *ok = false;
        return 0;
    }
    return p->param.get<float>(id, ok);
}

double signal_parameters_get_double(const signal_parameters_t *p,
                                    const char *id, bool *ok)
{
    if (!p || !id) {
        if (ok)
            *ok = false;
        return 0;
    }
    return p->param.get<double>(id, ok);
}

const char *signal_parameters_get_string(const signal_parameters_t *p,
                                         const char *id, bool *ok)
{
    if (!p || !id) {
        if (ok)
            *ok = false;
        return nullptr;
    }

    auto *str = p->param.get_direct(id, ok);
    if (str)
        return reinterpret_cast<std::string *>(str)->c_str();
    return nullptr;
}

const void *signal_parameters_get_data(const signal_parameters_t *p,
                                       const char *id, bool *ok)
{
    if (!p || !id) {
        if (ok)
            *ok = false;
        return nullptr;
    }
    return p->param.get_direct(id, ok);
}
