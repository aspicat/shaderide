# Changelog

You can find all changes in detail per version in this file.
The listed notes are not ordered by priority.

## Version 1.2.0 - December 6, 2019
### Added
- Toggle word wrap via "Code" menu.
- Highlight current cursor line in code editor (grey block).
- Scrollbar stylesheets.
- General settings dialog via "File/Settings..." now available.
- Log output counter with automatic cleanup.
- Realtime shader compilation when code has changed.
- Added state icons for toggleable menu entries.
- Added "Torus" model to preview models.

### Changed
- General code cleanup.
- Split stylesheet definitions into multiple headers.
- Models load in a separate thread now.
- Project files base on JSON now, instead of using boost::serialization.

### Fixed
- Layout sizes in splitter (width_left, width_right) were not saved for maximized window.
Fixed by applying layout settings first.

## Version 1.1.0 - December 2, 2019
### Added
#### GUI
- The output log is now toggleable.
- The application layout can now be stored.
- The main layout can be swapped (3D|Code -> Code|3D).
- The main layout can also be reset to default.
- Added various menu entries.
- Provided menu entry "Help" with an "About" box.
- Added torus preview model.

#### 3D Viewport
- Scaled down "Model Quick Load" icons in 3D viewport to 64 x 64 pixels.
- Added "Plane 2D" mode for flat shaders (fill viewport).
- In "Plane 2D" mode, "Model Quick Load" icons are hidden.
- In 3D viewport, left mouse double click resets the default model rotation and camera position (zoom).
- Removed dragon model.

#### Code Editor
- Zooming in code editors with Ctrl + Mouse Wheel Up/Down and with +/- keys on keyboard is now supported.
- Compilation errors and messages are now also logged to the status bar.
- GLSL errors are now highlighted in the code editors with a red overlay per line (simple).
- Cleaned up GLSL default variable names.

#### Project
- When a new project is created, show "Save Project as..." dialog when Ctrl + S is fired.
- Automatic compilation on save.
- The version number is now also available in project files for future version checks.
- Model translation is now saved with the project.
- Unsaved changes warning message box is now shown on exit.

#### Source Code
- Added license information to all C++ source files.
- Added resource file (resources.qrc) with binary data.
- Adjusted OBJMeshLoader to load meshes by binary resources.

### Changed
Not specified for this release. See "Added".

### Fixed
Not specified for this release. See "Added".

## Version 1.0.0 - November 30, 2019
For this first release are no notes available.
