<div id="user-content-toc">
  <ul align="center" style="list-style: none;">
    <summary>
      <h1>Nostalgia</h1>&nbsp;&nbsp;&nbsp;&nbsp;<h3>(Or, "How I learned to stop crashing and love stable code")</h3>
    </summary>
  </ul>
</div>

(LolBit texture stolen from [Puppet](https://neonrealm.carrd.co))
<img width="1920" src="https://github.com/user-attachments/assets/908d4e61-66da-4e3e-820f-74f1ae209cf8"/>

# Index
[A New Old Game Engine](#a-new-old-game-engine)<br>
[Current Changelog](#current-changelog)<br>
[Building Nostalgia](#building-nostalgia)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[Build Targets](#build-targets)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[Building the Library](#building-the-library)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[Building the Editor/Debugger](#building-the-editordebugger)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[Build Cleaning](#build-cleaning)<br>
[Theatre Files](#theatre-files)<br>
*Coming Soon...*

# A New Old Game Engine
My goal for creating the Nostalgia game engine is for it to eventually be capable of capturing the feeling of PS2-era video games, albiet with modern sensibilities and quality of life features added on top. This is a very ambitious goal, considering that it's only my second game engine, but it's a goal nonetheless.

# Current Changelog
Nothing yet...

# Building Nostalgia
I used my custom Makefile tool [MakeMake](https://github.com/Electron7-7/makemake) to generate the original Makefile for Nostalgia. The current Makefile is a heavily edited version but the core idea is the same: use certain targets almost like command line variables, and other targets as actual targets. There are "option" targets, a final "build" target, and some custom cleaning targets.
### Build Targets
Option targets specify certain properties for the final target(s). This includes things like the platform or version. These targets are: `linux`, `windows`, `debug`, `release`, `static`, `dynamic`, `compile_commands`, and `disable_colors`. The first six targets specify the platform, version, and library type while the `build` target actually builds the engine. You can run the option targets with the `clean` or `mostlyclean` targets if you only want to clean a specific build (e.g: running `make linux debug clean` cleans all the files under 'build/Linux/Debug/'); see [Build Cleaning](#build-cleaning) for more information on these two targets. `disable_colors` will remove all color from any output messages; this prevents the output from being filled with garbage on terminals that without ANSI support or piped output. `compile_commands` should be used when generating a 'compile_commands.json' file for clangd; without it, you'll run into two specific problems (I use LSP-clangd, but I'm pretty sure these problems happen to clangd regardless of how its being served). The first problem appears in [opengl_renderer_api.cpp](Engine/src/backends/opengl_renderer_api.cpp); clangd will crash while trying to parse a few lines that include embedded shader code variables, so a pre-processor macro is added to the compile commands that skips over them. The second problem is that clangd isn't able to understand pre-compiled header files, so the linker flag for including it is replaced with a compiler flag that includes the actual header file, [Nostalgia.hpp](Engine/Nostalgia.hpp), instead.
### Building the Library
Nostalgia compiles to either a static or dynamic C++ library, which can then be used to create games or applications. You can cross-compile a static Windows library (or use MSYS2/MINGW) but you can't compile a `.dll` or other dynamic Windows library *yet*. Static libraries are compiled as `.a` archive files and are compiled into the final binary. To compile the static library, use the target `static`; to compile a dynamic library, use the target `dynamic`. These targets will also build the required header files, which you can manually build with the target `headers`. I haven't yet written dedicated API headers, so `headers` will simply copy all the header files from the source directory into the output directory.
### Building the Editor/Debugger
You can also compile a test app for quick debugging, testing, or just to demo the engine. The testing app can be built to test the static library or the dynamic library, using `testapp_static` or `testapp_dynamic` respectively.
### Build Cleaning
There are two targets for cleaning the build directory: `clean` and `mostlyclean`. `clean` removes the entire build directory and `mostlyclean` removes everything except for certain `.obj`/`.o` files (their directories are defined in the variable `DIRTY_SRC_DIRS`). `mostlyclean` is useful for clean-building without re-compiling things like thirdparty files which don't change and don't need to be re-compiled. Both `clean` and `mostlyclean` can be run alone or with options; when run alone, they act on the entire build directory but when run with options, they act on the directory specified by the options. For example, running `make linux clean` will remove the directory `build/Linux` instead of the entire build directory. 

# Theatre Files
NostalgiaTheatre (also called "Theatre") files are based on my custom "GraphXTheatre" file format that I wrote for (GraphX)[https://github.com/Electron7-7/graphx]; they're essentially "game scene" files and are used to initialize a Theatre (scene). Nostalgia's Theatre files are much more robust, easier to write, and use a completely new lexer+parser that I wrote from scratch. Detailed documentation will be coming to this repository's wiki page soon; until then, the (HelloWorld.nt)[https://github.com/Electron7-7/nostalgia-game-engine/blob/trunk/theatres/HelloWorld.nt] file is heavily documented with plenty of examples and comments. Syntax highlighting for Sublime Text is available in this repo (and in its own repo soon) but shouldn't be too difficult to translate to your IDE of choice.
