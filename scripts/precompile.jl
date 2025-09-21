using JuliaC

JuliaC._main_cli([
    "--trim=safe",
    "--output-lib", "libgame.dylib",
    "scripts/main.jl"
])
