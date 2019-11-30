/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * SyntaxErrorException Class
 */

#ifndef SHADERIDE_CORE_SYNTAXERROREXCEPTION_HPP
#define SHADERIDE_CORE_SYNTAXERROREXCEPTION_HPP

#include "GeneralException.hpp"

namespace ShaderIDE {

    class SyntaxErrorException : public GeneralException {
    public:
        explicit SyntaxErrorException(const std::string &file,
                                      const std::string &message)
            : GeneralException(message),
              file(file)
        {}

        std::string File() {
            return file;
        }

    private:
        std::string file;
    };
}

#endif // SHADERIDE_CORE_SYNTAXERROREXCEPTION_HPP
