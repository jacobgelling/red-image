# Red Image
Program to handle textures and heightmaps from the 1995 title Big Red Racing.

The following file types from Big Red Racing are supported.
 * `.COL` colour palettes
 * `.MPH` heightmaps
 * `.RAW` textures
 * `.TM` textures (require a colour palette to be given)

## Usage
The latest binaries for Windows and Debian are available at [GitHub Releases](https://github.com/jacobgelling/red-image/releases/latest).

To decode a given image `DECALS.TM` with palette `DEFAULT.COL` to a GIF `decals.gif`, execute the following.
```bash
red-image -d DECALS.TM DEFAULT.COL decals.gif
```

To encode a given GIF `decals.gif` with palette `DEFAULT.COL` to a image `DECALS.TM`, execute the following.
```bash
red-image -e decals.gif DEFAULT.COL DECALS.TM
```

## Compilation
Compilation requires a C compiler and CMake.

To generate the build files, execute the following from the root project folder.
```bash
cmake . -B build
```

To build the project, execute the following from the root project folder.
```bash
cmake --build build
```

You can find the output binaries in the `bin` folder.
