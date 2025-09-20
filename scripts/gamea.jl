module GameA
using Dyno;

Base.@ccallable function _process(delta::Cdouble)::Cvoid
    println("GameA process: ", delta)
end

Dyno.@register GameA _process
end
