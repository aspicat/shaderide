/**
 * SerializableVector3 Class
 *
 * --------------------------------------------------------------------------
 * This file is part of "Shader IDE" -> https://github.com/aspicat/shaderide.
 * --------------------------------------------------------------------------
 *
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SHADERIDE_PROJECT_SERIALIZABLEVECTOR3_HPP
#define SHADERIDE_PROJECT_SERIALIZABLEVECTOR3_HPP

#include <glm/glm.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

namespace ShaderIDE::Project {

    class SerializableVector3 : public glm::vec3 {
        friend class boost::serialization::access;

        template <typename Archive>
        void serialize(Archive &ar, const uint32_t version) {
            ar & x;
            ar & y;
            ar & z;
        }

    public:
        explicit SerializableVector3(const glm::vec3 &vector)
            : glm::vec3(vector)
        {}

        explicit SerializableVector3(const float &scalar)
            : glm::vec3(scalar)
        {}

        SerializableVector3(const float &x,
                            const float &y,
                            const float &z)
            : glm::vec3(x, y, z)
        {}

        void operator= (const glm::vec3 &vector) {
            x = vector.x;
            y = vector.y;
            z = vector.z;
        }
    };
}

#endif // SHADERIDE_PROJECT_SERIALIZABLEVECTOR3_HPP
