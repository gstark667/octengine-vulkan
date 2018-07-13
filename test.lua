require("gameobject")
require("scene")

io.write("loading\n")

function setup()
    io.write("running script setup\n")
end

function update(scene, object, delta)
    gameobject_rotate(object, 0, 1 * delta, 0)
end

function on_cursor_pos(scene, object, x, y)
    io.write("cursor pos: ", x, ":", y, "\n")
end
