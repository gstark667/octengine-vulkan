mat4 rotate_bone(ivec4 idxs, vec4 weights, mat4 mats[64])
{
    if (idxs[0] == -1)
    {
        return mat4(1.0);
    }
    else
    {
        mat4 matrix = mats[idxs[0]] * weights[0];
        matrix += mats[idxs[1]] * weights[1];
        matrix += mats[idxs[2]] * weights[2];
        matrix += mats[idxs[3]] * weights[3];
        return matrix;
    }
}

mat3 rotate_euler(vec3 euler)
{
    mat3 mx, my, mz;
	
    // rotate around z
    float s = sin(euler.z);
    float c = cos(euler.z);
    mz[0] = vec3(c, s, 0.0);
    mz[1] = vec3(-s, c, 0.0);
    mz[2] = vec3(0.0, 0.0, 1.0);

    // rotate around y
    s = sin(-euler.y);
    c = cos(-euler.y);
    my[0] = vec3(c, 0.0, s);
    my[1] = vec3(0.0, 1.0, 0.0);
    my[2] = vec3(-s, 0.0, c);

    // rot around x
    s = sin(euler.x);
    c = cos(euler.x);	

    mx[0] = vec3(1.0, 0.0, 0.0);
    mx[1] = vec3(0.0, c, s);
    mx[2] = vec3(0.0, -s, c);

    return mz * my * mx;
}
