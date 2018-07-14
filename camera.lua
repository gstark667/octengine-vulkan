require("scene")
require("gameobject")

function setup(scene, object)
    scene_set_camera(scene, object)
    gameobject_set_transform(object, 0, 0, -5)
end

function update(scene, object, delta)

end

function on_cursor_pos(scene, object, x, y)
    new_x = x/100
    new_y = -y/100
    if new_y > 1.57079632679 then
        new_y = 1.57079632679
    elseif new_y < -1.57079632679 then
        new_y = 1.57079632679
    end
    gameobject_set_rotation(object, new_y, new_x, 0)
end
