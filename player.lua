require("scene")
require("gameobject")
require("physics")

function setup(scene, object)
    physics_init_capsule(scene, object, 10,  0.5, 2)
    physics_set_angular_factor(object, 0.0)

    camera = scene_add_gameobject(scene, "")
    scene_set_camera(scene, camera)
    gameobject_set_parent(camera, object)
    gameobject_set_transform(camera, 0, 1, 0)
    gameobject_set_integer(object, "camera", camera)

    gameobject_set_number(object, "acceleration", 100)
    gameobject_set_number(object, "max_velocity", 10)
    gameobject_set_number(object, "jump_speed", 5)

    gameobject_set_number(object, "x_axis", 0)
    gameobject_set_number(object, "z_axis", 0)

    gameobject_set_number(object, "x_rot", 0)
    gameobject_set_number(object, "y_rot", 0)
end

function dot(ax, ay, bx, by)
    ta = math.atan2(ay, ax)
    tb = math.atan2(by, bx)

    va = math.sqrt(ax*ax + ay*ay)
    vb = math.sqrt(bx*bx + by*by)

    return va * va * math.cos(math.abs(ta -tb))
end

function update(scene, object, delta)
    camera = gameobject_get_integer(object, "camera")

    x_rot = gameobject_get_number(object, "x_rot")
    y_rot = gameobject_get_number(object, "y_rot")
    physics_set_rotation(object, 0, x_rot, 0)
    gameobject_set_rotation(camera, y_rot, 0, 0)

    acceleration = gameobject_get_number(object, "acceleration")
    max_velocity = gameobject_get_number(object, "max_velocity")

    x_axis = gameobject_get_number(object, "x_axis")
    y_axis = gameobject_get_number(object, "y_axis")
    gameobject_set_number(object, "y_axis", 0)
    z_axis = gameobject_get_number(object, "z_axis")
    wish_vel = math.sqrt(x_axis * x_axis + z_axis * z_axis)

    x_cur_vel, y_cur_vel, z_cur_vel = physics_get_velocity(object)
    cur_vel = math.sqrt(x_cur_vel * x_cur_vel + z_cur_vel * z_cur_vel)

    -- dot product
    proj_vel = dot(x_axis, z_axis, x_cur_vel, z_cur_vel) --(x_axis * x_cur_vel + z_axis * z_cur_vel) / (x_axis * x_axis + z_axis * z_axis)
    accel_vel = acceleration * delta

    if proj_vel + accel_vel > max_velocity then
        accel_vel = max_velocity - proj_vel
    end

    x_accel = (math.cos(x_rot) * x_axis - math.sin(x_rot) * z_axis) * accel_vel
    z_accel = (math.sin(x_rot) * x_axis + math.cos(x_rot) * z_axis) * accel_vel
    x_vel = x_cur_vel + x_accel
    z_vel = z_cur_vel + z_accel

    y_vel = y_axis * gameobject_get_number(object, "jump_speed") + y_cur_vel

    physics_set_velocity(object, x_vel, y_vel, z_vel)
    --physics_set_velocity(object, x_speed, y, z_speed)
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
        z_axis = gameobject_get_number(object, "z_axis")
        gameobject_set_number(object, "z_axis", z_axis - 1)
    elseif button_code == "S" then
        z_axis = gameobject_get_number(object, "z_axis")
        gameobject_set_number(object, "z_axis", z_axis + 1)
    elseif button_code == "A" then
        x_axis = gameobject_get_number(object, "x_axis")
        gameobject_set_number(object, "x_axis", x_axis - 1)
    elseif button_code == "D" then
        x_axis = gameobject_get_number(object, "x_axis")
        gameobject_set_number(object, "x_axis", x_axis + 1)
    elseif button_code == "Space" then
        gameobject_set_number(object, "y_axis", 1)
    end
end

function on_button_up(scene, object, button_code)
    if button_code == "W" then
        z_axis = gameobject_get_number(object, "z_axis")
        gameobject_set_number(object, "z_axis", z_axis + 1)
    elseif button_code == "S" then
        z_axis = gameobject_get_number(object, "z_axis")
        gameobject_set_number(object, "z_axis", z_axis - 1)
    elseif button_code == "A" then
        x_axis = gameobject_get_number(object, "x_axis")
        gameobject_set_number(object, "x_axis", x_axis + 1)
    elseif button_code == "D" then
        x_axis = gameobject_get_number(object, "x_axis")
        gameobject_set_number(object, "x_axis", x_axis - 1)
    end
end
