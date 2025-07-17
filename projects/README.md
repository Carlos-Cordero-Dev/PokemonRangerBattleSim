=== NX ===

For this to work some hard coded variables have to be changed:

premake5.lua: 		"D:/DevkitPro/libnx/include", --libnx headers
Makefile: 	RAYLIB_NX=D:/PokemonRangerBattleSim 		(its just the root of the project, replace D: for whatever)

Note: 
for the whole thing to work:

use https://github.com/eavpsp/raylib-nx-opengl4.2 
as a base for the project, then update
glad with
https://github.com/eavpsp/Glad-NX-NV_gpu_shader5-ext-4.2
by changing src_nx/external/glad.h to the one in the repo.
Also devkitpro/portlibs/switch/include/glad.h and lib/libglad.a have
to be the ones fromn the repo

libraylib.a has to be located at rootofthisproject/src , which is generated doing 
example:
from PokemonBattleSim/src_nx

changes makefile:

RAYLIB_SRC_PATH      ?= ../src_nx

ifeq ($(PLATFORM),PLATFORM_NX)

    GRAPHICS = GRAPHICS_API_OPENGL_42
endif

and clfags:

    ifeq ($(PLATFORM),PLATFORM_NX)
        CFLAGS += -O2 -DGRAPHICS_API_OPENGL_42
    endif

make PLATFORM=PLATFORM_NX

=== WINDOWS ===

raylib_win_d.lib has to be located at rootofthisproject/sr, which is generated doing:
(from the root of the project):
-mkdir build cd build
(THE FOLLOWING STEP MUST BE DONE FROM Visual Studio Developer Command Prompt, in this case for VS2019 and the propper generators)
-cmake -G "Visual Studio 16 2019" -A x64 -DPLATFORM=Desktop -DBUILD_SHARED_LIBS=OFF -DGRAPHICS=GRAPHICS_API_OPENGL_42 ..
-open raylib.sln and compile
-the "raylib.lib" at \build\raylib\Debug or Release have to be renamed to raylib_win_d(debug in this case).lib and moved to src/