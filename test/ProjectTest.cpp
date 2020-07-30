/**
 * Project Test
 *
 * -------------------------------------------------------------------------------
 * This file is part of "Shader IDE" -> https://github.com/thedamncoder/shaderide.
 * -------------------------------------------------------------------------------
 *
 * Copyright (c) 2017 - 2020 Florian Roth
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

#define BOOST_TEST_MODULE ProjectTest
#include <iostream>
#include <boost/test/unit_test.hpp>
#include "src/Project/ShaderProject.hpp"

using namespace ShaderIDE::Project;

BOOST_AUTO_TEST_SUITE(ProjectTestSuite)

BOOST_AUTO_TEST_CASE(ProjectTestCase)
{
    const std::string WINDOWS_PATH_ONLY = "C:\\Windows\\Paths\\Are\\Ugly\\";
    const std::string PATH_ONLY = "/Path/To/Project/";
    const std::string NAME = "project";
    const std::string EXTENSION = ".shaderide";
    const std::string PATH = PATH_ONLY + NAME + EXTENSION;
    const std::string WINDOWS_PATH = WINDOWS_PATH_ONLY + NAME + EXTENSION;
    const std::string VS_SOURCE = "VERTEX_SHADER_CODE";
    const std::string FS_SOURCE = "FRAGMENT_SHADER_CODE";
    const std::string MESH_NAME = "Mesh";

    // const std::string TEXTURE_SLOT_0 = "tex0";
    // const std::string TEXTURE_SLOT_1 = "tex1";
    // const std::string TEXTURE_SLOT_2 = "tex2";
    // const std::string TEXTURE_SLOT_3 = "tex3";

    ShaderProject project(PATH);
    project.SetVertexShaderSource(VS_SOURCE);
    project.SetFragmentShaderSource(FS_SOURCE);
    project.SetMeshName(MESH_NAME);
    project.SetRealtime(true);
    project.SetPlane2D(true);

    BOOST_ASSERT(project.Version() == SHADERIDE_VERSION);
    BOOST_ASSERT(project.Path() == PATH);
    BOOST_ASSERT(project.VertexShaderSource() == VS_SOURCE);
    BOOST_ASSERT(project.FragmentShaderSource() == FS_SOURCE);
    BOOST_ASSERT(project.MeshName() == MESH_NAME);
    BOOST_ASSERT(project.Realtime());
    BOOST_ASSERT(project.Plane2D());
    BOOST_ASSERT(project.ModelRotation() == OPENGLWIDGET_DEFAULT_MODEL_ROTATION);
    BOOST_ASSERT(project.CameraPosition() == OPENGLWIDGET_DEFAULT_CAMERA_POSITION);

    // Path Check
    BOOST_ASSERT(project.PathOnly() == PATH_ONLY);

    project.SetPath(WINDOWS_PATH);
    BOOST_ASSERT(project.PathOnly() == WINDOWS_PATH_ONLY);

    project.SetPath(NAME + EXTENSION);
    BOOST_ASSERT(project.PathOnly() == "./");
}

BOOST_AUTO_TEST_SUITE_END()
