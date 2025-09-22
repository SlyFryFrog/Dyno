using Dyno

include("gamea.jl")
include("gameb.jl")

Dyno.generate_wrappers(true)
