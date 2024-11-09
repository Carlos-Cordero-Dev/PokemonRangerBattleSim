
-- premake5.lua
workspace "MyProject"
    -- Define two configurations: Windows and Switch
    configurations { "Switch", "Windows" }

-- Project configuration
project "MyProject"
    kind "Makefile"
	architecture "x64"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"  -- Output directory for binaries

    -- Project source and include files
    files { "source/**.c", "include/**.h" }

    -- Include directories
    includedirs {
        "../src",                -- raylib headers
        "include",                -- project headers
    }

    -- Filter for Switch configuration
    filter "configurations:Switch"
		system "linux"           -- Set as Linux-like environment for make
		defines { "SWITCH_BUILD" }

		buildcommands { "make" }
		rebuildcommands { "make clean", "make" }
		cleancommands { "make clean" }

		libdirs { 
			"../src",                -- nx raylib libs (.a)
		}

		links { "raylib" }
	    includedirs {
			"D:/DevkitPro/libnx/include", --libnx headers
		}
		-- Custom debug command
		debugcommand "$(ProjectDir)\\tools\\run_suyu.bat"
		buildoutputs { "bin/%{cfg.buildcfg}/myproject.nro" }  -- Example for output file

    filter "configurations:Windows"
		kind "ConsoleApp"
		defines { "WINDOWS_BUILD" , "DEBUG"}
        symbols "On"	--debug for now
		
		cdialect "C99"
        cppdialect "C++17"

		
		includedirs {
			"../src",                -- raylib headers
			"../src/external",
			"../src/external/glfw/include",
			
			"include",                -- project headers
		}
		libdirs {
				"../src", -- windows raylib libs (.lib)
		}
		links { "raylib_win_d", "winmm", "gdi32" }

