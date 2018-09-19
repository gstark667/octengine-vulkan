require("gameobject")
require("scene")

function setup()
end

function update(scene, object, delta)
    gameobject_rotate(object, 1 * delta, 1 * delta, 1 * delta)
end
