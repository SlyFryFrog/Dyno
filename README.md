# Dyno

## Overview


### Dyno AOT

```julia
using JuliaC
JuliaC._main_cli(["--output-lib", "cmake-build-debug/libgame.dylib", "scripts/game.jl"])
```

### Dyno REPL