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

#include <iostream>
#include <libsignal.h>
#include <string>

struct rect {
    int width, height, x, y;
};

void signal_1(const signal::parameters &param, signal::parameters *)
{
    std::cout << "Int argument 'int_test': " << param.get<int>("int_test")
              << std::endl;
    std::cout << "String argument 'str_test':"
              << param.get<std::string>("str_test") << std::endl;
}

void signal_2(const signal::parameters &param, signal::parameters *output)
{
    bool ok = false;
    std::cout << "Float argument 'float_test': "
              << param.get<float>("float_test") << std::endl;
    std::cout << "Double argument 'double_test': "
              << param.get<double>("double_test") << std::endl;

    const rect &r = param.get<rect>("rect", &ok);

    if (ok) {
        std::cout << "Rectangle: " << r.width << "x" << r.height << " at "
                  << r.x << ", " << r.y << std::endl;
    }

    if (output) {
        output->add<bool>("signal_2_success", true);
    }
}

class receiver_a : public signal::receiver
{
    std::string m_name;

  public:
    receiver_a() = default;
    receiver_a(const std::string &name) : m_name(name) {}

    void receive(const signal::parameters & = signal::parameters(),
                 signal::parameters * = nullptr) override
    {
        std::cout << "Object " << m_name << " signal fired" << std::endl;
    }
};

int main()
{
    rect rectangle = {1280, 720, 42, 393};
    auto obj = std::make_shared<receiver_a>("receiver_a");

    signal::manager m;
    signal::parameters input, output;

    input.add<int>("int_test", 1337);
    input.add<std::string>("str_test", std::string("Test string aaaa"));
    input.add<float>("float_test", 4444.324223f);
    input.add<double>("double_test", 12345667.346347345);
    input.add<rect>("rect", rectangle);

    m.add("signal_1", signal_1);
    m.add("signal_2", signal_2);
    m.add("signal_2", obj);

    m.send("signal_1", input);
    m.send("signal_2", input, &output);

    std::cout << "Signal 2 sucess: "
              << (output.get<bool>("signal_2_success") ? "true" : "false")
              << std::endl;

    return 0;
}
