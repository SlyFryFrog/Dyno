# Dyno

Dyno is a library for interfacing with Julia from C++.


## Overview

### Dyno AOT

```julia
using JuliaC

JuliaC._main_cli([
    "--trim=safe",
    "--output-lib", "mylib.dylib",  # Or .so for Linux
    "scripts/mymain.jl"
])
```

### Dyno REPL