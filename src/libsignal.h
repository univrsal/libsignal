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

#ifndef LIB_SIGNAL_H
#define LIB_SIGNAL_H

#ifdef __cplusplus /* C++ Implementation */
#include <algorithm>
#include <cstring>
#include <malloc.h>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace signal {

/**
 * \brief The parameters class, contains a list of parameters used for calling signals
 * \class parameters
 */
class parameters {
    std::map<std::string, std::pair<size_t, void *>> m_parameters;

    public:
    parameters() = default;

    ~parameters()
    {
        for (auto &param : m_parameters) {
            free(param.second.second);
            param.second.second = nullptr;
            param.second.first = 0;
        }
    }

    /**
     * \brief Add any variable to the list
     * \param T the variable type
     * \param id the id of the parameter
     * \param param the value of the variable
     * \return true if the variable could be added, false if it already exists
     * \defgroup signal++
     */
    template<class T> bool add(const std::string &id, const T &param)
    {
        auto tmp = m_parameters.find(id);
        if (tmp == m_parameters.end()) {
            m_parameters[id].second = new T(param);
            m_parameters[id].first = sizeof(T);
            return true;
        }
        return false;
    }

    /**
     * \brief Add a data pointer to the list
     * \param id the id of the parameter
     * \param data the data to add
     * \param s the size of the pointer
     * \return true if the variable could be added, false if it already exists
     * \defgroup signal++
     */
    bool add_direct(const std::string &id, const void *data, size_t s)
    {
        auto tmp = m_parameters.find(id);
        if (tmp == m_parameters.end()) {
            void *d = malloc(s);
            memcpy(d, data, s);
            m_parameters[id].second = d;
            m_parameters[id].first = s;
            return true;
        }
        return false;
    }

    /**
     * \brief Get a variable from the list
     * \param id the id of the parameter
     * \param ok will be set to true on success (optional)
     * \param def the default return value on failure (optional)
     * \return the value of the parameter
     * \defgroup signal++
     */
    template<class T>
    const T &get(const std::string &id, bool *ok = nullptr,
                 const T &def = T()) const
    {
        auto p = m_parameters.find(id);
        if (p == m_parameters.end()) {
            if (ok)
                *ok = false;
            return def;
        }
        if (sizeof(T) != p->second.first) {
            if (ok)
                *ok = false;
            return def;
        }
        if (ok)
            *ok = true;
        return *reinterpret_cast<T *>(p->second.second);
    }

    /**
     * \brief Get a data pointer from the list
     * \param id the id of the parameter
     * \param ok will be set to true on success (optional)
     * \param def the default return value on failure (optional)
     * \return the value of the parameter, this is a direct pointer and should be copied
     * \defgroup signal++
     */
    void *get_direct(const std::string &id, bool *ok = nullptr,
                     void *def = nullptr) const
    {
        auto p = m_parameters.find(id);
        if (p == m_parameters.end()) {
            if (ok)
                *ok = false;
            return def;
        }
        if (ok)
            *ok = true;
        return p->second.second;
    }
};

/**
 * \brief The receiver class is an interface class, which can be implemented
 * to allow sending of signals to objects
 * \class receiver
 * \defgroup signal++
 */
class receiver {
    public:
    virtual ~receiver() = default;
    virtual void receive(const parameters &param = parameters(),
                         parameters *out = nullptr) = 0;
};

/**
 * \typedef Function pointer
 * \brief Structure for any method called by a signal
 * \param params    Parameters sent to this method by the signal caller
 * \param repsonse    Output data for this signal (shared by all methods)
 * \defgroup signal++
 */
typedef void (*signal_function)(const parameters &param, parameters *response);

/**
 * \brief The signal class holds all receivers for this signal
 * they are separated in receiver methods and receiver objects
 * \class signal
 * \defgroup signal++
 */
class signal {
    std::vector<signal_function> m_receivers;
    std::vector<std::shared_ptr<receiver>> m_receiver_objects;
    public:
    signal() = default;

    /**
     * \brief Adds f as the first receiver
     * \param f the first receiver for this signal
     * \defgroup signal++
     */
    signal(signal_function f) { m_receivers.emplace_back(f); }

    /**
     * \brief Adds r as the first receiver object
     * \param r the first receiver object for this signal
     * \defgroup signal++
     */
    signal(receiver *r)
    {
        if (r)
            m_receiver_objects.emplace_back(std::shared_ptr<receiver>(r));
    }

    /**
     * \brief Adds r as the first receiver object using a shared pointer
     * to ensure that the object exists as long as this signal does
     * \param r the first receiver object for this signal
     * \defgroup signal++
     */
    signal(std::shared_ptr<receiver> &&r) { m_receiver_objects.emplace_back(r); }

    /**
     * \brief Invoke this signal
     * \param param the paramters to send to the receivers (optional)
     * \param response the response used by the receivers (shared by all receivers) (optional)
     * \defgroup signal++
     */
    void invoke(const parameters &param = parameters(),
                parameters *response = nullptr) const
    {
        for (const auto &recv : m_receivers)
            recv(param, response);
        for (const auto &recv_obj : m_receiver_objects)
            recv_obj->receive(param, response);
    }

    /**
     * \brief Add a receiver for this signal
     * \param f the receiver function
     * \return true if the function could be added, false if it is already registered
     * \defgroup signal++
     */
    bool add_receiver(signal_function f)
    {
        if (!f)
            return false;
        if (std::find(m_receivers.begin(), m_receivers.end(), f) ==
            m_receivers.end()) {
            m_receivers.emplace_back(f);
            return true;
        }
        return false;
    }

    /**
     * \brief Add a receiver object for this signal using a shared pointer
     * to ensure that the object exists for as long as this signal does
     * \param r the receiver object
     * \return true if the receiver object could be added, false if it is already registered
     * \defgroup signal++
     */
    bool add_receiver_obj(std::shared_ptr<receiver> &&r)
    {
        if (!r)
            return false;
        if (std::find(m_receiver_objects.begin(), m_receiver_objects.end(),
                      r) == m_receiver_objects.end()) {
            m_receiver_objects.emplace_back(r);
            return true;
        }
        return false;
    }
};

/**
 * \brief The manager class, manages signals
 * \class manager
 * \defgroup signal++
 */
class manager {
    std::map<std::string, signal> m_signals;

    public:
    manager() = default;
    ~manager() = default;

    /**
     * \brief Send a signal to all receivers
     * \param id the id of the signal to invoke
     * \param param the parameters to send to the receivers
     * \param response the response parameters used by the receivers (shared by all receivers)
     * \return true if the signal could be found, otherwise false
     * \defgroup signal++
     */
    bool send(const std::string &id, const parameters &param = parameters(),
              parameters *response = nullptr) const
    {
        auto sig = m_signals.find(id);
        if (sig == m_signals.end())
            return false;
        sig->second.invoke(param, response);
        return true;
    }

    /**
     * \brief Add a signal to the manager
     * \param id the id of the signal to register
     * \param fun the first receiver function for this signal (optional)
     * \return true if the signal could be added, false if the id already exists
     * \defgroup signal++
     */
    bool add(const std::string &id, signal_function fun = nullptr)
    {
        auto sig = m_signals.find(id);
        if (sig == m_signals.end()) {
            m_signals[id] = signal(fun);
            return true;
        }
        if (fun)
            return sig->second.add_receiver(fun);
        return false;
    }

    /**
     * \brief Add an object signal to the manager using a shared pointer
     * to make sure that the object exists as long as the signal
     * \param id the id of the signal to register
     * \param r the receiver object for this signal
     * \return true if the signal could be added, false if the id already exists
     * \defgroup signal++
     */
    bool add(const std::string &id, std::shared_ptr<receiver>&& r)
    {
        auto sig = m_signals.find(id);
        if (sig == m_signals.end()) {
            m_signals[id] = signal(std::move(r));
            return true;
        }
        return sig->second.add_receiver_obj(std::move(r));
    }
};
};

#endif /* C++ implementation */

/* C Definition */

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

/**
 * \struct signal_manager_t csignal.h
 * \brief Opaque structure which holds C++ implementation of the signal handler
 * \defgroup signal++
 */
typedef struct signal_manager_s signal_manager_t;

/**
 * \struct signal_paramters_t csignal.h
 * \brief Opaque structure which holds C++ implementation of signal parameters
 * \defgroup signal++
 */
typedef struct signal_parameters_s signal_parameters_t;

/**
 * \typedef Function pointer
 * \brief Structure for any method called by a signal
 * \param params	Parameters sent to this method by the signal caller
 * \param repsonse	Output data for this signal (shared by all methods)
 * \defgroup signal++
 */
typedef void (*signal_function_t)(const signal_parameters_t *params,
                                  signal_parameters_t *response);

/**
 * \brief Create a new signal manager, free with signal_manager_free
 * \return A new signal manager
 * \defgroup signal++
 */
extern DECLSPEC signal_manager_t *C_SIGNAL_CALL signal_manager_create(void);

/**
 * \brief Frees a signal manager created with signal_manager_create
 * \param m the signal manager to free
 * \defgroup signal++
 */
extern DECLSPEC void C_SIGNAL_CALL signal_manager_free(signal_manager_t *m);

/**
 * \brief Create new signal parameters, free with signal_parameters_free
 * \return New signal parameters
 * \defgroup signal++
 */
extern DECLSPEC signal_parameters_t *C_SIGNAL_CALL
signal_parameters_create(void);

/**
 * \brief Frees signal parameters created with signal_parameters_create
 * \param p the parameters to free
 * \defgroup signal++
 */
extern DECLSPEC void C_SIGNAL_CALL
signal_parameters_free(signal_parameters_t *p);

/**
 * \brief Send a signal to all registered handlers
 * \param m the signal manager to use
 * \param id the id of the signal to invoke
 * \param param the parameters with which to call the function, can be NULL
 * \param out the output parameters for the handler methods, can be NULL
 * \return true on success, false if either id or m is NULL
 * \defgroup signal++
 */
extern DECLSPEC bool C_SIGNAL_CALL signal_send(signal_manager_t *m,
                                               const char *id,
                                               const signal_parameters_t *param,
                                               signal_parameters_t *out);

/**
 * \brief Register a new signal for this signal manager
 * \param m the signal manager to use
 * \param id the id of the signal to register
 * \param fun the signal handler function
 * \return true on success, false if m, id or fun is NULL or if the function is already registered
 * \defgroup signal++
 */
extern DECLSPEC bool C_SIGNAL_CALL signal_add(signal_manager_t *m,
                                              const char *id,
                                              signal_function_t fun);

/**
 * \brief Add an integer variable to the parameter list
 * \param p the parameter list to use
 * \param id the id of the parameter
 * \param val the value of the parameter
 * \return true on success, false if p or id is NULL or if the variable already exists
 * \defgroup signal++
 */
extern DECLSPEC bool C_SIGNAL_CALL
signal_parameters_set_int(signal_parameters_t *p, const char *id, int val);

/**
 * \brief Add an unsigned integer variable to the parameter list
 * \param p the parameter list to use
 * \param id the of the parameter
 * \param val the value of the parameter
 * \return true on success, false if p or id is NULL or if the variable already exists
 * \defgroup signal++
 */
extern DECLSPEC bool C_SIGNAL_CALL signal_parameters_set_uint(
        signal_parameters_t *p, const char *id, unsigned int val);

/**
 * \brief Add a boolean variable to the parameter list
 * \param p the parameter list to use
 * \param id the id of the parameter
 * \param val the value of the parameter
 * \return true on success, false if p or id is NULL or if the variable already exists
 * \defgroup signal++
 */
extern DECLSPEC bool C_SIGNAL_CALL
signal_parameters_set_bool(signal_parameters_t *p, const char *id, bool val);

/**
 * \brief Add a float variable to the parameter list
 * \param p the parameter list to use
 * \param id the id of the parameter
 * \param val the value of the parameter
 * \return true on success, false if p or id is NULL or if the variable already exists
 * \defgroup signal++
 */
extern DECLSPEC bool C_SIGNAL_CALL
signal_parameters_set_float(signal_parameters_t *p, const char *id, float val);

/**
 * \brief Add a double variable to the parameter list
 * \param p the parameter list to use
 * \param id the id of the parameter
 * \param val the value of the parameter
 * \return true on success, false if p or id is NULL or if the variable already exists
 * \defgroup signal++
 */
extern DECLSPEC bool C_SIGNAL_CALL signal_parameters_set_double(
        signal_parameters_t *p, const char *id, double val);

/**
 * \brief Add a string variable to the parameter list
 * \param p the parameter list to use
 * \param id the id of the parameter
 * \param val the value of the parameter
 * \return true on success, false if p or id is NULL or if the variable already exists
 * \defgroup signal++
 */
extern DECLSPEC bool C_SIGNAL_CALL signal_parameters_set_string(
        signal_parameters_t *p, const char *id, const char *val);

/**
 * \brief Add any data to the parameter list
 * \param p the parameter list to use
 * \param id the id of the parameter
 * \param val the value of the parameter
 * \param size the size in bytes of the data pointer
 * \return true on success, false if p or id is NULL or if the variable already exists
 * \defgroup signal++
 */
extern DECLSPEC bool C_SIGNAL_CALL signal_parameters_set_data(
        signal_parameters_t *p, const char *id, void *val, size_t size);

/**
 * \brief Get an integer variable from the parameter list
 * \param p the parameter list to use
 * \param id the id of the parameter
 * \param ok will be set to true if the variable could be retrieved, can be NULL
 * \return the value of the variable
 * \defgroup signal++
 */
extern DECLSPEC int C_SIGNAL_CALL signal_parameters_get_int(
        const signal_parameters_t *p, const char *id, bool *ok);

/**
 * \brief Get an unsigned integer variable from the parameter list
 * \param p the parameter list to use
 * \param id the id of the parameter
 * \param ok will be set to true if the variable could be retrieved, can be NULL
 * \return the value of the variable
 * \defgroup signal++
 */
extern DECLSPEC unsigned int C_SIGNAL_CALL signal_parameters_get_uint(
        const signal_parameters_t *p, const char *id, bool *ok);

/**
 * \brief Get a boolean variable from the parameter list
 * \param p the parameter list to use
 * \param id the id of the parameter
 * \param ok will be set to true if the variable could be retrieved, can be NULL
 * \return the value of the variable
 * \defgroup signal++
 */
extern DECLSPEC bool C_SIGNAL_CALL signal_parameters_get_bool(
        const signal_parameters_t *p, const char *id, bool *ok);

/**
 * \brief Get a float variable from the parameter list
 * \param p the parameter list to use
 * \param id the id of the parameter
 * \param ok will be set to true if the variable could be retrieved, can be NULL
 * \return the value of the variable
 * \defgroup signal++
 */
extern DECLSPEC float C_SIGNAL_CALL signal_parameters_get_float(
        const signal_parameters_t *p, const char *id, bool *ok);

/**
 * \brief Get a double variable from the parameter list
 * \param p the parameter list to use
 * \param id the id of the parameter
 * \param ok will be set to true if the variable could be retrieved, can be NULL
 * \return the value of the variable
 * \defgroup signal++
 */
extern DECLSPEC double C_SIGNAL_CALL signal_parameters_get_double(
        const signal_parameters_t *p, const char *id, bool *ok);

/**
 * \brief Get a string variable from the parameter list
 * \param p the parameter list to use
 * \param id the id of the parameter
 * \param ok will be set to true if the variable could be retrieved, can be NULL
 * \return the value of the variable
 * \defgroup signal++
 */
extern DECLSPEC const char *C_SIGNAL_CALL signal_parameters_get_string(
        const signal_parameters_t *p, const char *id, bool *ok);

/**
 * \brief Get any data variable from the parameter list
 * \param p the parameter list to use
 * \param id the id of the parameter
 * \param ok will be set to true if the variable could be retrieved, can be NULL
 * \return the value of the variable, this is a direct pointer and should be copied if needed
 * \defgroup signal++
 */
extern DECLSPEC const void *C_SIGNAL_CALL signal_parameters_get_data(
        const signal_parameters_t *p, const char *id, bool *ok);

#ifdef __cplusplus
}
#endif /* extern "c" */

#endif /* Header guard */
