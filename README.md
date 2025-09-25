<div id="user-content-toc">
  <ul align="center" style="list-style: none;">
    <summary>
      <h1>Nostalgia</h1>&nbsp;&nbsp;&nbsp;&nbsp;<h3>(Or, "How I learned to stop crashing and love stable code")</h3>
    </summary>
  </ul>
</div>

(LolBit texture stolen from [Puppet](https://neonrealm.carrd.co))
<img width="1920" src="https://github.com/user-attachments/assets/9f14e781-0658-43e2-8c02-26b42febb103"/>

# Index
[A New Old Game Engine](#a-new-old-game-engine)<br>
[Building Nostalgia](#building-nostalgia)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[Option Targets](#option-targets)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[Final Targets](#final-targets)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[Building the Library](#building-the-library)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[Building the Testing App](#building-the-testing-app)<br>
&nbsp;&nbsp;&nbsp;&nbsp;[Cleaning the Build Directory](#cleaning-the-build-directory)<br>
*Coming Soon...*

# A New Old Game Engine
My goal for creating the Nostalgia game engine is for it to eventually be capable of capturing the feeling of PS2-era video games, albiet with modern sensibilities and quality of life features added on top. At the time of writing it's only been in development for exactly 4 months and it's already far surpassed [GraphX](https://github.com/Electron7-7/graphx) in terms of stability, robustness, quality, and future-proofing. It's currently in a state where I feel comfortable enough making a more detailed README file.

# Building Nostalgia
I used my custom Makefile tool [MakeMake](https://github.com/Electron7-7/makemake) to generate the original Makefile for Nostalgia. The current Makefile is a heavily edited version but the core idea is the same: use certain targets almost like command line variables, and other targets as actual targets. There are 6 "option" targets, 7 final targets, and 2 targets for cleaning.
### Option Targets
Option targets specify certain properties for the final target(s). This includes things like the platform or version. These targets are: `linux`, `windows`, `debug`, `release`, `disable_colors`, and `clangd`. The first four targets specify the target platform and version; the last two are more unique. `disable_colors` will remove all color from any output messages (this prevents the output from being filled with garbage on terminals that without ANSI support or piped output). `clangd` will add `-D CLANGD_KEEPS_CRASHING_HERE` to the C++ compile command and should be used when generating a `compile_commands.json` file; this is because clangd will consistently run into problems in `src/rendering/backends/graphics/opengl.cpp` on lines 50 and 51. I don't know the *exact* reason why, but I assume it's related to my specific usage of embedded string arrays via `#embed` (and/or passing them to a function the way I do).
### Final Targets
Final targets are what you use to actually build things. You can build the Nostalgia library and the testing application with these targets. Both the library and the app have static and dynamic versions with associated Makefile targets. These targest are: `static`, `dynamic`, `testapp_static`, and `testapp_dynamic`. There are also the targets `libraries` and `testapps` which will make both versions of their respective target.
### Building the Library
Nostalgia compiles to either a static or dynamic C++ library, which can then be used to create games or applications. You can cross-compile a static Windows library (or use MSYS2/MINGW) but you can't compile a `.dll` or other dynamic Windows library *yet*. Static libraries are compiled as `.a` archive files and are compiled into the final binary. To compile the static library, use the target `static`; to compile a dynamic library, use the target `dynamic`. These targets will also build the required header files, which you can manually build with the target `headers`. I haven't yet written dedicated API headers, so `headers` will simply copy all the header files from the source directory into the output directory.
### Building the Testing App
You can also compile a test app for quick debugging, testing, or just to demo the engine. The testing app can be built to test the static library or the dynamic library, using `testapp_static` or `testapp_dynamic` respectively.
### Cleaning the Build Directory
There are two targets for cleaning the build directory: `clean` and `mostlyclean`. `clean` removes the entire build directory and `mostlyclean` removes everything except for certain `.obj`/`.o` files (their directories are defined in the variable `DIRTY_SRC_DIRS`). `mostlyclean` is useful for clean-building without re-compiling things like thirdparty files which don't change and don't need to be re-compiled. Both `clean` and `mostlyclean` can be run alone or with options; when run alone, they act on the entire build directory but when run with options, they act on the directory specified by the options. For example, running `make linux clean` will remove the directory `build/Linux` instead of the entire build directory. 
