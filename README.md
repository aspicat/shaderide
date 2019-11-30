[logo]: assets/app/logo.png "ShaderIDE Logo"
![alt text][logo]

# Shader IDE
Simple GLSL code editor with realtime 3D preview.

[screeny]: assets/app/screenshot.jpg "ShaderIDE Screenshot"
![alt text][screeny]

The project was initialized during OpenGL learning sessions to have a better
understanding of GLSL shaders with realtime preview. There are already a couple of
web solutions online, but I wanted a native application for Windows and Linux with
an export function to use my shaders also in other projects and to have the ability
to extend the application and make backups of my shader projects.

## Releases
v0.1.0 - First release with very basic functionality.

## Build
You need the following libraries:
* Qt5 Core, Gui, Widgets, OpenGL - 5.13.0
* Boost Libraries - 1.69
* OpenGL Mathematics (GLM) - 0.9.9

See CMakeLists.txt for further instructions.

## Usage
The whole editor should be quite intuitive. On the left side you can find the
3D preview, on the right side is the code section. On the bottom you can find
the log output.

The 3D viewport can be controlled with the mouse. Left mouse button rotates the object.
Keep the wheel (middle mouse button) pressed and move up and down the mouse to translate
the model back and forth. With the right mouse button clicked you can move the camera.

Note the image icon at the bottom right corner of the code editor. You may
select textures (tex0 - tex3) for the four predefined sampler2D uniforms, which
may be used for albedo, normal, metalness and roughness for example. In future
versions could be more than the four predefined slots, so you can add them
dynamically as needed.

### Keyboard Shortcuts
| Command      | Description                                       |
|--------------|---------------------------------------------------|
| Ctrl + R     | Compile shader code and apply to 3D scene.        |
| Ctrl + S     | Save currently opened project.                    |
