module GameB
using Dyno;

Base.@ccallable function _process(delta::Cdouble)::Cvoid
    println(Core.stdout, "Delta: ", delta)
end

function my_func(arg1::Int32, arg2::Int32, arg3::Float64)
    println(Core.stdout, "Args: ", arg1, " ", arg2, " ", arg3)
end

Dyno.@register GameB _process
Dyno.@register GameB my_func
end
