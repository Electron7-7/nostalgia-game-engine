<div id="user-content-toc">
  <ul align="center" style="list-style: none;">
    <summary>
      <h1>Nostalgia</h1>&nbsp;&nbsp;&nbsp;&nbsp;<h3>(Or, "How I learned to stop crashing and love stable code")</h3>
    </summary>
  </ul>
</div>

(LolBit texture stolen from [Puppet](https://neonrealm.carrd.co))
<img width="1920" src="https://github.com/user-attachments/assets/908d4e61-66da-4e3e-820f-74f1ae209cf8"/>

### Check out the [wiki](https://github.com/Electron7-7/nostalgia-game-engine/wiki) for more info, guides, and documentation!

# A New Old Game Engine
My goal for creating the Nostalgia game engine is for it to eventually be capable of capturing the feeling of PS2-era video games, albiet with modern sensibilities and quality of life features added on top. This is a very ambitious goal, considering that it's only my second game engine, but it's a goal nonetheless.

# Building Nostalgia
#### Note for Windows Users: I'm still figuring out the Windows build, so for now it's Linux only. The CMakeLists.txt file *does* generate a valid Visual Studio solution & project files, and it all compiles without many issues (using ClangCL), but linking the testing application throws some strange errors. Once I get it fully working, I'll make another pre-release (or maybe just a v0.x release).
Nostalgia uses CMake but don't worry, this section was written with the assumption that you know nothing about CMake. If you do, however, know something about CMake, then the only thing you need to know is that the build directories are set by the presets and you should use them instead of setting the directory yourself. Up to you, though. Oh, and `BUILD_SHARED_LIBS` is how I differentiate between building the shared and static versions of the library, if you *really* wanna go manual-mode.

One last thing to note: on Linux, you'll need to install `directx-shader-compiler` for Jolt to compile. However, at least on Arch and as of the time of writing, you may run into a seg-fault when compiling. If this happens, you'll need to build `directx-shader-compiler` from its git repo (or use `yay -S directx-shader-compiler-git` on Arch). You could also try setting the cmake entry `JPH_USE_DXC` to `OFF` and re-generating the build files, as this *should* force Jolt to use `fxc` instead of `dxc`.

#### Configuring Nostalgia
Building Nostalgia via CMake GUI is exceptionally easy to do; just set the source directory to the repository (e.g: where `CMakeLists.txt` is located) and select your preset from the 'presets' dropdown. After that, you'll notice a build directory will be chosen for you; this is intentional and should not be changed (you *can* if you want to, but the `.gitignore` file won't ignore it so be mindful). After pressing 'configure' once, a *lot* of options will pop up; most of these are from the included libraries that Nostalgia depends on and shouldn't be configured (they'll be overridden internally, anyways). Currently, there aren't any options to set that aren't set by the presets (e.g: static/dynamic library, debug/release version); if I add any in the future, I'll also try to hide the thirdparty options and write up some documentation for them. If you want to generate a `compile_commands.json` file, add a boolean entry named `CMAKE_EXPORT_COMPILE_COMMANDS` and enable it (e.g: set it to 'ON'). Oh, and in case you didn't know: if you wanna set the compiler (say, to 'clang++'), you'll want to add/set the entry `CMAKE_CXX_COMPILER` to your compiler of choice.

Once you've pressed 'configure' twice and all the entries are no longer red, press 'generate' and wait for it to finish. Now, your build directory is prepared and you're ready to build and install Nostalgia (as well as the editor/testing app).

#### Actually Building Nostalgia
Actually building Nostalgia on Linux is pretty simple; after you've configured the build system using CMake, navigate to the repository in a terminal and run the command `cmake --build <build_directory>`. For example: to build the debug linux version (either static or dynamic) you would run `cmake --build ./Linux_Debug` (unless you manually specified a different build directory). If you wish to install Nostalgia (and its headers) after building it, run `cmake --install <build_directory>` to use the default installation location (typically `/usr/local/lib`) or `cmake --install <build_directory> --prefix <output_location>` to customize the installation location.

Building Nostalgia on Windows is a little less *and* a little more complicated. The initial process is very similar to Linux; use CMake to configure and generate the Visual Studio solution & project files. Then, open the solution file in Visual Studio 18. Nostalgia requires 'ClangCL' to be installed; to install clang, install the 'C++ Clang Compiler for Windows' and 'MSBuild support for LLVM (clang-cl) toolset' components via the Visual Studio updater. Once you have all of that, you *should* be able to just open and build the solution to get your files. Now, the Windows build is still untested as of writing this so the library may not be working on Windows yet, but this will be how you compile it if/when it's fully working.

# Using Nostalgia
First things first, Nostalgia uses the [Jolt physics engine](https://github.com/jrouwe/JoltPhysics) and since the task of fully abstracting Jolt into my own highly detailed physics implementation is... daunting to say the least, you'll need to build and include Jolt's header files yourself (or, if you're lazy like me, add `Nostalgia/thirdparty/JoltPhysics/Jolt/` to your include directories). You also need to include `<Nostalgia/Nostalgia.hpp>` before including pretty much any other header, due to it being the library's pre-compiled header. If you're using a pre-compiled header, it'd speed up compilation a lot (and save your sanity) if you add the it to your project's pre-compiled header. This is similar to Jolt's `<Jolt/Jolt.h>` file. Finally, if you want a quick and easy GUI, check out the editor/testing app as it demonstrates how to implement Nostalgia's UI interface/solution framework. Specifically, `Editor/gui/imgui_implementor.cpp` (and its accompanied header) can be copy-pasted into your own project for a quick and dirty implementation of ImGui into Nostalgia. The editor/testing app (located in `Editor/`) is Nostalgia's 'Hello World' program and a great tool for showcasing how to build an app/game using the engine.
