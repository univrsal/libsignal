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

#ifndef C_SIGNAL_H
#define C_SIGNAL_H

#include "types.h"
#include <stdbool.h>
#include <stddef.h>

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

#endif
