require("gameobject")
require("scene")

function setup()
end

function update(scene, object, delta)
    gameobject_rotate(object, 0, 1 * delta, 0)
end
