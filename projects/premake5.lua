
-- premake5.lua
workspace "MyProject"
    -- Define two configurations: Windows and Switch
    configurations { "Switch", "Windows" }

-- Project configuration
project "MyProject"
	kind "Makefile"
	architecture "x64"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"  -- Output directory for binaries

    -- Project source and include files
    files { "source/**.c", "source/**/**.c",
			"source/**.cpp","source/**/**.cpp",
			"include/**.h" ,"include/**/**.h",
			"include/**.hpp" ,"include/**/**.hpp"}

    -- Include directories
    includedirs {
        "include", "include/**", "include/**/**"                -- project headers
    }
		
    -- Filter for Switch configuration
    filter "configurations:Switch"
	    kind "Makefile"
		system "linux"           -- Set as Linux-like environment for make
		defines { "SWITCH_BUILD" , "DEBUG", "NX_USB_DEBUGGER"}

		buildcommands {"C:\\devkitpro\\msys2\\usr\\bin\\sh.exe -c \"make\""}
		rebuildcommands { "C:\\devkitpro\\msys2\\usr\\bin\\sh.exe -c \"make clean\"", "C:\\devkitpro\\msys2\\usr\\bin\\sh.exe -c \"make\""}
		cleancommands { "C:\\devkitpro\\msys2\\usr\\bin\\sh.exe -c \"make clean\"" }

		libdirs { 
			"../src_nx",                -- nx raylib libs (.a)
		}

		links { "raylib" }
	    includedirs {
			"../src_nx", 
			"../src_nx/external", 
			"D:/devkitPro/libnx/include", --libnx headers
		}
		-- Custom debug command
		debugcommand "$(ProjectDir)\\tools\\run_suyu.bat"
		buildoutputs { "bin/%{cfg.buildcfg}/myproject.nro" }  -- Example for output file

    filter "configurations:Windows"
		kind "ConsoleApp"
		defines { "WINDOWS_BUILD" , "DEBUG"}
        symbols "On"	--debug for now
		
		cppdialect "C++17"
		cdialect "C99"

		
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
		buildoptions { "/TP" } -- For MSVC to treat all files as C++ bc if not everything c++ breaks