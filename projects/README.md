For this to work some hard coded variables have to be changed:

premake5.lua: 		"D:/DevkitPro/libnx/include", --libnx headers
Makefile: 	RAYLIB_NX=D:/PokemonRangerBattleSim 		(its just the root of the project, replace D: for whatever)


Note: 
for the whole thing to work:

libraylib.a has to be located at rootofthisproject/src , which is generated doing make PLATFORM=PLATFORM_NX
raylib_win.lib has to be located at rootofthisproject/sr, which is generated doing:
(from the root of the project):
-mkdir build cd build
(THE FOLLOWING STEP MUST BE DONE FROM Visual Studio Developer Command Prompt, in this case for VS2019 and the propper generators)
-cmake -G "Visual Studio 16 2019" -A x64 -DPLATFORM=Desktop -DBUILD_SHARED_LIBS=OFF ..
-open raylib.sln and compile
-the "raylib.lib" at \build\raylib\Debug or Release have to be renamed to raylib_win_d(debug in this case).lib and moved to src/