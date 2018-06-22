require("gameobject")
require("scene")

io.write("loading\n")

function setup()
    io.write("running script setup\n")
end

function update(scene, object, delta)
    gameobject_transform(object, 1 * delta, 0, 0)
    gameobject_rotate(object, 0, 1 * delta, 0)
    io.write("it can call any lua functions: ", tostring(delta), "\n")
end
