[logo]: assets/app/logo.png "ShaderIDE Logo"
![alt text][logo]

# Shader IDE
Simple GLSL code editor with realtime 3D preview.

[screeny]: assets/app/screenshot_v1_2_0.jpg "ShaderIDE Screenshot"
![alt text][screeny]

The project was initialized during OpenGL learning sessions to have a better
understanding of GLSL shaders with realtime preview. There are already a couple of
web solutions online, but I wanted a native application for Windows and Linux with
an export function to use my shaders also in other projects and to have the ability
to extend the application and make backups of my shader projects.

## Warning
The application is in a very early state. Upcoming changes may break your projects!

## Releases
See CHANGELOG.md for detailed release notes.

## Build
You need the following libraries:
* Qt5 Core, Gui, Widgets, OpenGL - 5.13.0
* Boost Libraries - 1.69
* OpenGL Mathematics (GLM) - 0.9.9

See CMakeLists.txt for further instructions.

## Usage
The whole editor should be quite intuitive. On the left side you can find the
3D preview, on the other side is the code section. The output log is positioned at the
bottom of the main window.

The 3D viewport can be controlled with the mouse. Left mouse button rotates the object.
Keep middle mouse pressed and move up and down the mouse to translate
the model back and forth. With the right mouse button pressed you can move the camera.

Note the gear icon at the bottom right corner of the code editor. You may
select textures (tex0 - tex3) for the four predefined sampler2D uniforms, which
may be used for albedo, normal, metalness and roughness for example. In future
versions could be more than the four predefined slots, so you can add them
dynamically as needed.

### Keyboard Shortcuts
| Command           | Description                                       |
|-------------------|---------------------------------------------------|
| Ctrl + R          | Compile shader code and apply to 3D scene.        |
| Ctrl + Alt + R    | Toggle realtime compilation.                      |
| Ctrl + S          | Save currently opened project.                    |
| Ctrl + L          | Toggle log output view.                           |
| Ctrl + -/+        | Zoom text in code editors.                        |

## Shader Variables
The following predefined shader variables are supported as of version 1.2.0:

### Vertex Shader
* **in vec3 position**
* **in vec3 normal**
* **in vec2 uv**

### Fragment Shader
* **uniform sampler2D tex0**
* **uniform sampler2D tex1**
* **uniform sampler2D tex2**
* **uniform sampler2D tex3**

### Uniforms (All Shaders)
* **uniform mat4 modelMat**
* **uniform mat4 viewMat**
* **uniform mat4 projectionMat**
* **uniform float time**
