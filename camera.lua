require("scene")
require("gameobject")

function setup(scene, object)
    scene_set_camera(scene, object)
    gameobject_set_transform(object, 0, 0, -5)
    gameobject_set_number(object, "forward", 0)
    gameobject_set_number(object, "back", 0)
    gameobject_set_number(object, "left", 0)
    gameobject_set_number(object, "right", 0)
    gameobject_set_number(object, "x_rot", 0)
    gameobject_set_number(object, "y_rot", 0)
end

function update(scene, object, delta)
    x_rot = gameobject_get_number(object, "x_rot")
    y_rot = gameobject_get_number(object, "y_rot")
    gameobject_set_rotation(object, y_rot, x_rot, 0)

    x_speed = (gameobject_get_number(object, "left") - gameobject_get_number(object, "right")) * delta * 3
    z_speed = (gameobject_get_number(object, "forward") - gameobject_get_number(object, "back")) * delta * 3
    x_vel = math.cos(x_rot) * x_speed - math.sin(x_rot) * z_speed
    z_vel = math.sin(x_rot) * x_speed + math.cos(x_rot) * z_speed
    gameobject_transform(object, x_vel, 0, z_vel)
end

function on_cursor_pos(scene, object, x, y)
    new_x = x/250 + gameobject_get_number(object, "x_rot")
    new_y = -y/250 + gameobject_get_number(object, "y_rot")
    if new_y > 1.57079632679 then
        new_y = 1.57079632679
    elseif new_y < -1.57079632679 then
        new_y = -1.57079632679
    end
    gameobject_set_number(object, "x_rot", new_x)
    gameobject_set_number(object, "y_rot", new_y)
end

function on_button_down(scene, object, button_code)
    if button_code == "W" then
        gameobject_set_number(object, "forward", 1)
    elseif button_code == "S" then
        gameobject_set_number(object, "back", 1)
    elseif button_code == "A" then
        gameobject_set_number(object, "left", 1)
    elseif button_code == "D" then
        gameobject_set_number(object, "right", 1)
    end
end

function on_button_up(scene, object, button_code)
    if button_code == "W" then
        gameobject_set_number(object, "forward", 0)
    elseif button_code == "S" then
        gameobject_set_number(object, "back", 0)
    elseif button_code == "A" then
        gameobject_set_number(object, "left", 0)
    elseif button_code == "D" then
        gameobject_set_number(object, "right", 0)
    end
end
