io.write("loading\n")

function setup()
    io.write("running script setup\n")
end

function update(delta)
    io.write("running script delta: ", tostring(delta), "\n")
end
