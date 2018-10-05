require("scene")
require("gameobject")
require("camera")
require("physics")

function setup(scene, object)
    physics_init_capsule(scene, object, 1,  0.5, 1)
    physics_set_angular_factor(object, 0.0)

    camera = scene_add_camera(scene)
    scene_set_camera(scene, camera)
    camera = camera_get_gameobject(camera)
    gameobject_set_parent(camera, object)
    gameobject_set_transform(camera, 0, 0.5, 0)
    gameobject_set_integer(object, "camera", camera)

    gameobject_set_integer(object, "grounded", 0)
    gameobject_set_number(object, "ground_acceleration", 5)
    gameobject_set_number(object, "air_acceleration", 1)
    gameobject_set_number(object, "friction", 5)
    gameobject_set_number(object, "max_velocity", 10)
    gameobject_set_number(object, "jump_speed", 7)
    gameobject_set_number(object, "gravity", -9.8)

    gameobject_set_number(object, "x_axis", 0)
    gameobject_set_number(object, "z_axis", 0)

    gameobject_set_number(object, "x_rot", 0)
    gameobject_set_number(object, "y_rot", 0)

    gameobject_set_number(object, "x_look", 0)
    gameobject_set_number(object, "y_look", 0)
end

function dot(x, y)
    local sum = 0
    for i = 1, #x do
        sum = sum + x[i] * y[i]
    end
    return sum
end

function update(scene, object, delta)
    camera = gameobject_get_integer(object, "camera")

    x_rot = gameobject_get_number(object, "x_rot") + gameobject_get_number(object, "x_look")
    y_rot = gameobject_get_number(object, "y_rot") + gameobject_get_number(object, "y_look")
    if y_rot > 1.57079632679 then
        y_rot = 1.57079632679
    elseif y_rot < -1.57079632679 then
        y_rot = -1.57079632679
    end
    gameobject_set_number(object, "x_rot", x_rot)
    gameobject_set_number(object, "y_rot", y_rot)

    physics_set_rotation(object, 0, -x_rot, 0)
    gameobject_set_rotation(camera, -y_rot, 0, 0)

    friction = gameobject_get_number(object, "friction")
    max_velocity = gameobject_get_number(object, "max_velocity")
    grounded = gameobject_get_integer(object, "grounded")
    jump_speed = gameobject_get_number(object, "jump_speed")
    gravity = gameobject_get_number(object, "gravity")

    acceleration = 0
    if grounded > 0 then
        acceleration = gameobject_get_number(object, "ground_acceleration")
    else
        acceleration = gameobject_get_number(object, "air_acceleration")
    end

    x_axis = gameobject_get_number(object, "x_axis")
    z_axis = gameobject_get_number(object, "z_axis")
    y_axis = gameobject_get_number(object, "y_axis")
    gameobject_set_number(object, "y_axis", 0)
    if grounded == 0 then
        y_axis = 0
    end

    cur_x, cur_y, cur_z = physics_get_velocity(object)
    cur_y = cur_y + gravity * delta + y_axis * jump_speed
    cur_vel = math.sqrt(cur_x * cur_x + cur_z * cur_z)

    input_vel = math.sqrt(x_axis * x_axis + z_axis * z_axis)
    if input_vel > 1 then
        x_axis = x_axis / input_vel
        z_axis = z_axis / input_vel
        input_vel = 1
    end
    x_mag = math.cos(x_rot) * x_axis - math.sin(x_rot) * z_axis
    z_mag = math.sin(x_rot) * x_axis + math.cos(x_rot) * z_axis

    wish_speed = input_vel * max_velocity
    current_speed = dot({cur_x, cur_z}, {x_mag, z_mag})
    add_speed = wish_speed - current_speed

    if math.abs(cur_vel) > 0 and grounded > 0 then
        drop = cur_vel * friction * delta
        percent = math.max(cur_vel - drop, 0) / cur_vel
        cur_x = cur_x * percent
        cur_z = cur_z * percent
    end

    if add_speed > 0 then
        accel_speed = acceleration * delta * wish_speed
        if accel_speed > add_speed then
            accel_speed = add_speed
        end

        physics_set_velocity(object, cur_x + accel_speed * x_mag, cur_y, cur_z + accel_speed * z_mag)
    else
        physics_set_velocity(object, cur_x, cur_y, cur_z)
    end
end

function on_cursor_pos(scene, object, x, y)
    new_x = x/250 + gameobject_get_number(object, "x_rot")
    new_y = y/250 + gameobject_get_number(object, "y_rot")
    gameobject_set_number(object, "x_rot", new_x)
    gameobject_set_number(object, "y_rot", new_y)
end

function on_event(scene, object, name, value)
    if name == "x_axis" or name == "z_axis" or name =="y_axis" then
        gameobject_set_number(object, name, value)
    elseif name == "x_look" or name == "y_look" then
        gameobject_set_number(object, name, value)
    end
end

function on_collision_enter(scene, object, other)
    count = gameobject_get_integer(object, "grounded")
    gameobject_set_integer(object, "grounded", count + 1)
end

function on_collision_exit(scene, object, other)
    count = gameobject_get_integer(object, "grounded")
    gameobject_set_integer(object, "grounded", count - 1)
end
