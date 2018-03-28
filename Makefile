all:
	cd shaders; glslangValidator -V shader.vert; glslangValidator -V shader.frag
