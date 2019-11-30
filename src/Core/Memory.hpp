/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * Memory Class
 */

#ifndef SHADERIDE_CORE_MEMORY_HPP
#define SHADERIDE_CORE_MEMORY_HPP

namespace ShaderIDE {

    class Memory {
    public:
        template <typename T>
        static void Release(T *t) {
            delete t;
            t = nullptr;
        }
    };
}

#endif // SHADERIDE_CORE_MEMORY_HPP
