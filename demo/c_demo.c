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

#include <libsignal.h>
#include <stdio.h>
#define UNUSED_PARAMETER(p) ((void)p)

typedef struct rect_s {
    int width, height, x, y;
} rect_t;

void signal_1(const signal_parameters_t *param, signal_parameters_t *output)
{
    UNUSED_PARAMETER(output);
    printf("Int argument 'int_test': %i\n",
           signal_parameters_get_int(param, "int_test", NULL));
    printf("String argument 'str_test': %s\n",
           signal_parameters_get_string(param, "str_test", NULL));
}

void signal_2(const signal_parameters_t *param, signal_parameters_t *output)
{
    bool ok = false;
    printf("Float argument 'float_test': %f\n",
           (double)signal_parameters_get_float(param, "float_test", NULL));
    printf("Double argument 'double_test': %f\n",
           signal_parameters_get_double(param, "double_test", NULL));

    const rect_t *r = signal_parameters_get_data(param, "rect", &ok);
    if (ok) {
        printf("Rectangle: %ix%i at %i, %i\n", r->width, r->height, r->x, r->y);
    }

    if (output) {
        signal_parameters_set_bool(output, "signal_2_success", true);
    }
}

int main()
{
    signal_manager_t *manager = signal_manager_create();
    signal_parameters_t *input = signal_parameters_create();
    signal_parameters_t *output = signal_parameters_create();
    rect_t rectangle = {.x = 42, .y = 393, .width = 1280, .height = 720};

    signal_parameters_set_int(input, "int_test", 1337);
    signal_parameters_set_string(input, "str_test", "Test string aaaa");
    signal_parameters_set_float(input, "float_test", 4444.324223f);
    signal_parameters_set_double(input, "double_test", 12345667.346347345);
    signal_parameters_set_data(input, "rect", &rectangle, sizeof(rect_t));

    signal_add(manager, "signal_1", signal_1);
    signal_add(manager, "signal_2", signal_2);

    signal_send(manager, "signal_1", input, NULL);
    signal_send(manager, "signal_2", input, output);

    printf("Signal 2 sucess: %s\n",
           signal_parameters_get_bool(output, "signal_2_success", NULL)
               ? "true"
               : "false");

    signal_parameters_free(input);
    signal_parameters_free(output);
    signal_manager_free(manager);
    return 0;
}
