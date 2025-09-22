using JuliaC

ext = Sys.iswindows() ? ".dll" : Sys.isapple() ? ".dylib" : ".so"
lib_name = "libgame" * ext

JuliaC._main_cli([
    "--trim=safe",
    "--output-lib", lib_name,
    "scripts/main.jl"
])
