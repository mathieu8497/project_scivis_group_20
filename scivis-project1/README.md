# SciVis

[![build](https://github.com/peterdanieloliver/scivis/actions/workflows/build.yaml/badge.svg)](https://github.com/peterdanieloliver/scivis/actions/workflows/build.yaml)

## Building

CMake is used as the build system. A version of 3.5 or higher is required.

The general steps to build any CMake project are as follows:

1. Make a build directory (e.g. `build`)
2. `cd` into the build directory and run `cmake ..`, which will generate the build system
3. You may then use the generated build system to build the project, or perform a platform agnostic build with `cmake --build .` (note the `.`, which specifies the *build* directory, not the *source* directory as in **step 2**)

You may specify a specific build system by naming one with the `-G` switch when running `cmake ..`. The syntax is as follows:

```bash
cmake -G <generator> ..
```

**Note:** You may see a list of supported generators by running `cmake -G`.

The project requires an installation of [glut](https://www.opengl.org/resources/libraries/glut/) and [glew](https://glew.sourceforge.net/) to build. See the below subsections for help.

### Windows

The newest version of Visual Studio installed on your computer is usually the default build system. A Visual Studio solution will be created in your build directory.

If using Visual Studio, you may build by right-clicking the `learnply` project and clicking **Build**.

An installation of glut and glew is distributed with this repository and is located in the `libraries` subdirectory.

### Mac/Linux

Unix Makefiles are usually the default build system. If using this generator, you may build the project by running `make` from within the build directory.

You may use `gdb` or `lldb` to debug the program on Linux and Mac, respectively. Altenatively, you may use another build system with a built in debugger, such as Xcode on Mac.

If using Xcode, the `.xcodeproj` will be created in the build directory.

glut installation varies per platform, see below.

#### Mac

freeglut and glew can be installed using [Homebrew](https://brew.sh/). Once installed, run:

```
brew install freeglut
brew install glew
```

and you should be good to go.

#### Linux

Some Linux distributions are distributed with a version of freeglut and glew, so installation may not be required. Refer to your system's documentation to install it. On Ubuntu freeglut and glew can be installed with

```
sudo apt-get install freeglut3-dev
sudo apt-get install libglew-dev
```

## Running

The program expects a single command line argument specifying the file to display.

### Windows

You must first set the learnply project to be run. Do this by right-clicking the `learnply` project and clicking **Set as Startup Project**.

You must specify the path to the ply file in your Project Settings. Do this by right-clicking the `learnply` *project*, navigating the the **Debug** tab, and in the **Command line arguments** field, enter the path to the ply file.

You may now run the project by pressing **F5** or clicking the **Start** button.

**Note:** that if your file path has spaces in it, it must be surrounded by quotes.

### Mac/Linux

If the CMake generator was Unix Makefiles, the executable will be created in your build directory. From within that directory, simply run

```bash
./learnply "path/to/file.ply"
```

#### Xcode

If using Xcode, you must set the scheme you want to run. The current scheme is displayed at the top of the window, near the center. It will likely say `learnply`, `ZERO_CHECK`, or `ALL_BUILD`. Click on that text, then select `learnply`.

Now, to set the argument, click on the text again, then click **Edit Scheme**. Next, navigate to the **Run** section and click on the **Arguments** tab. Then, under **Arguments Passed On Launch**, click the plus symbol and insert the path in the text box.

## FAQ

### Windows

* If getting a build error relating to `setlocal`, check to make sure all the required `dll` and `lib` files are in the respective subdirectories in the `libraries` directory.

### Mac

* The window is unclosable and can only be closed using **Cmd+Q** while it is focused.
