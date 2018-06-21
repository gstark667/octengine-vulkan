require("gameobject")

io.write("loading\n")

function setup()
    io.write("running script setup\n")
end

function update(object, delta)
    io.write("running script delta: ", tostring(delta), "\n")
    io.write("running script object: ", tostring(object), "\n")
    gameobject_transform(object, 0.01 * delta, 0, 0)
end
