add_rules("mode.debug", "mode.release", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

add_requires("fmt", "libsdl", "libsdl_image", "entt", "nlohmann_json", "dr_wav")
add_requires("openal-soft", { configs = { shared = true }})
add_requires("imgui", { configs = { sdl2 = true, sdl2_renderer = true }})
--add_requireconfs("libsdl", "**.libsdl", {configs = { sdlmain = is_mode("distrib")}})
add_requireconfs("libsdl", "**.libsdl", {configs = { sdlmain = false}})
add_requires("catch2")
add_requires("enet6")

set_project("NeelaEngine")

set_allowedmodes("debug", "releasedbg", "release", "distrib")

set_languages("cxx20")
set_exceptions("cxx")
set_encodings("utf-8")
set_rundir(".")
set_warnings("allextra")
add_includedirs("include")
add_installfiles("(assets/**)", { prefixdir = "bin" })

add_cxflags("/wd4251")

if is_mode("distrib") then
    set_optimize("fastest")
    set_symbols("none")
else
    add_defines("NEELA_WITH_EDITOR")
    if is_mode("debug") then
        set_suffixname("-debug")
    end
end

target("NeelaEngine")
    if is_mode("distrib") then
        set_kind("static")
        add_defines("NEELA_ENGINE_STATIC", {public = true})
    else
        set_kind("shared")
    end

    add_headerfiles("include/Neela/**.h", "include/Neela/**.inl")
    add_files("src/Neela/**.cpp")
    add_packages("fmt", "libsdl", "libsdl_image", "imgui", "openal-soft", "entt", "dr_wav", "nlohmann_json", { public = true })
    add_defines("NEELA_ENGINE_BUILD")

target("CoreLib")
    set_group("Common")

    if is_mode("distrib") then
        add_ldflags("/subsystem:windows")
        add_defines("AC_DISTRIB")
    end

    set_kind("static")
    add_headerfiles("include/Common/sh_**.h", "include/Common/**/sh_**.h")
    add_headerfiles("include/Common/sh_**.inl", "include/Common/**/sh_**.inl")
    add_files("src/Common/sh_**.cpp", "src/Common/**/sh_**.cpp")
    add_packages("enet6", {public = true})
    add_deps("NeelaEngine")

target("ClientLib")
    set_group("Common")

    if is_mode("distrib") then
        add_ldflags("/subsystem:windows")
        add_defines("AC_DISTRIB")
    end
    
    set_kind("static")
    add_files("src/resources.rc")
    add_headerfiles("include/Common/cl_**.h", "include/Common/**/cl_**.h")
    add_headerfiles("include/Common/cl_**.inl", "include/Common/**/cl_**.inl")
    add_files("src/Common/cl_**.cpp", "src/Common/**/cl_**.cpp")
    add_deps("CoreLib")

target("ServerLib")
    set_group("Common")

    if is_mode("distrib") then
        add_ldflags("/subsystem:windows")
        add_defines("AC_DISTRIB")
    end
    
    set_kind("static")
    add_headerfiles("include/Common/sv_**.h", "include/Common/**/sv_**.h")
    add_headerfiles("include/Common/sv_**.inl", "include/Common/**/sv_**.inl")
    add_files("src/Common/sv_**.cpp", "src/Common/**/sv_**.cpp")
    add_deps("CoreLib")

target("AC")
    if is_mode("distrib") then
        add_ldflags("/subsystem:windows")
        add_defines("AC_DISTRIB")
    end

    set_kind("binary")
    add_headerfiles("include/Game/sh_**.h", "include/Game/sh_**.inl")
    add_headerfiles("include/Game/cl_**.h", "include/Game/cl_**.inl")
    add_files("src/Game/sh_**.cpp", "src/Game/cl_**.cpp")
    add_deps("ClientLib", "ServerLib", "NeelaEngine")

target("ACServer")
    if is_mode("distrib") then
        add_ldflags("/subsystem:windows")
        add_defines("AC_DISTRIB")
    end
    set_kind("binary")
    add_headerfiles("include/Game/sh_**.h", "include/Game/sh_**.inl")
    add_headerfiles("include/Game/sv_**.h", "include/Game/sv_**.inl")
    add_files("src/Game/sh_**.cpp", "src/Game/sv_**.cpp")
    add_deps("ServerLib", "NeelaEngine")

for _, file in ipairs(os.files("**/Tests/**.cpp")) do
    local name = path.basename(file)
    target(format("Tests-%s", name))
        set_group("Tests")
        set_kind("binary")
        set_default(false)
        add_files(file)
        add_tests(name)
        add_packages("catch2")
        add_deps("NeelaEngine", "CoreLib")
end

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--