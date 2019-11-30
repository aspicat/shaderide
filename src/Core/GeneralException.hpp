/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * GeneralException Class
 */

#ifndef SHADERIDE_CORE_GENERALEXCEPTION_HPP
#define SHADERIDE_CORE_GENERALEXCEPTION_HPP

#include <exception>
#include <string>

namespace ShaderIDE {

    class GeneralException : public std::exception {
    public:
        explicit GeneralException(const std::string &message)
            : std::exception(),
              message(message)
        {}

        [[nodiscard]]
        const char* what() const noexcept override {
            return message.c_str();
        }

    private:
        std::string message;
    };
}

#endif // SHADERIDE_CORE_GENERALEXCEPTION_HPP
