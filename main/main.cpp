#include <iostream>

#include <fmt/format.h>

#include <ytcpp/core/error.hpp>
#include <ytcpp/core/js.hpp>
using namespace ytcpp;

int main() {
    try {
        Js::Interpreter interpreter;
        while (true) {
            fmt::print("> ");
            std::string input;
            std::getline(std::cin, input);

            if (input == "reset") {
                interpreter.reset();
                continue;
            }

            try {
                std::string output = interpreter.execute(input);
                fmt::print("{}\n", output);
            }
            catch (const Js::Error& error) {
                fmt::print(stderr, "[{}]\n", error.what());
            }
        }
    }
    catch (const Error& error) {
        fmt::print(stderr, "Fatal ytcpp::Error!\n");
        fmt::print(stderr, "{}\n", error.what());
        return 1;
    }
}
