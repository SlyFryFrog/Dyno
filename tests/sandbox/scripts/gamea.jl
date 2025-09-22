using Dyno;

module GameA

num::Int = 0

function _init()::Int
    global num
    num = num + 10
    return 3
end

function _process(delta::Float64)::Float64
    delta
    string = "Hello from Julia"
    return delta
end

function my_func(hour::Float64, minute::Float64)::Nothing

end

function my_func_int(hour)
    return hour
end

end

Dyno.@register GameA _init
Dyno.@register GameA _process   # Exports to C-interface as julia_GameA__process
Dyno.@register GameA my_func
Dyno.@register GameA my_func_int
