module GameB
using Dyno;

Base.@ccallable function _process(delta::Cdouble)::Cvoid
    println("GameB process: ", delta)
end

Dyno.@register GameB _process
end
