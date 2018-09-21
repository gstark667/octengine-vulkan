all:
	x86_64-w64-mingw32-g++ --shared -fPIC -std=c++11 -g -Wall -DGLM_FORCE_RADIANS -DGLM_FORCE_DEPTH_ZERO_TO_ONE src/util.cpp src/shader.cpp src/image.cpp src/texture.cpp src/physics.cpp src/pipeline.cpp src/gameobject.cpp src/buffer.cpp src/model.cpp src/camera.cpp src/script.cpp src/application.cpp lualibs/gameobject.cpp -Iinclude -L./ -llua -lvulkan-1 -lSDL2 -lFreeImage -lBulletCollision -lBulletDynamics -lLinearMath -lassimp -loctengine -o gameobject.dll
	x86_64-w64-mingw32-g++ --shared -fPIC -std=c++11 -g -Wall -DGLM_FORCE_RADIANS -DGLM_FORCE_DEPTH_ZERO_TO_ONE src/util.cpp src/shader.cpp src/image.cpp src/texture.cpp src/physics.cpp src/pipeline.cpp src/gameobject.cpp src/buffer.cpp src/model.cpp src/camera.cpp src/script.cpp src/application.cpp lualibs/scene.cpp -Iinclude -L./ -llua -lvulkan-1 -lSDL2 -lFreeImage -lBulletCollision -lBulletDynamics -lLinearMath -lassimp -loctengine -o scene.dll
	x86_64-w64-mingw32-g++ --shared -fPIC -std=c++11 -g -Wall -DGLM_FORCE_RADIANS -DGLM_FORCE_DEPTH_ZERO_TO_ONE src/util.cpp src/shader.cpp src/image.cpp src/texture.cpp src/physics.cpp src/pipeline.cpp src/gameobject.cpp src/buffer.cpp src/model.cpp src/camera.cpp src/script.cpp src/application.cpp lualibs/physics.cpp -Iinclude -L./ -llua -lvulkan-1 -lSDL2 -lFreeImage -lBulletCollision -lBulletDynamics -lLinearMath -lassimp -loctengine -o physics.dll
	x86_64-w64-mingw32-g++ --shared -fPIC -std=c++11 -g -Wall -DGLM_FORCE_RADIANS -DGLM_FORCE_DEPTH_ZERO_TO_ONE src/util.cpp src/shader.cpp src/image.cpp src/texture.cpp src/physics.cpp src/pipeline.cpp src/gameobject.cpp src/buffer.cpp src/model.cpp src/camera.cpp src/script.cpp src/application.cpp -Iinclude -llua -lvulkan-1 -lSDL2 -lFreeImage -lBulletCollision -lBulletDynamics -lLinearMath -lassimp -o liboctengine.dll
	x86_64-w64-mingw32-g++ -fPIC -std=c++11 -g -Wall -DGLM_FORCE_RADIANS -DGLM_FORCE_DEPTH_ZERO_TO_ONE src/util.cpp src/shader.cpp src/image.cpp src/texture.cpp src/physics.cpp src/pipeline.cpp src/gameobject.cpp src/buffer.cpp src/model.cpp src/camera.cpp src/script.cpp src/application.cpp src/main.cpp -Iinclude -llua -lvulkan-1 -lSDL2 -lFreeImage -lBulletCollision -lBulletDynamics -lLinearMath -lassimp -o octengine.exe
	cp /usr/x86_64-w64-mingw32/lib/FreeImage.dll ./
	cp /usr/x86_64-w64-mingw32/bin/libBulletCollision.dll ./
	cp /usr/x86_64-w64-mingw32/bin/libBulletDynamics.dll ./
	cp /usr/x86_64-w64-mingw32/bin/libLinearMath.dll ./
	cp /usr/x86_64-w64-mingw32/bin/SDL2.dll ./
	cp /usr/x86_64-w64-mingw32/bin/libassimp.dll ./
	cp /usr/x86_64-w64-mingw32/bin/libgcc_s_seh-1.dll ./
	cp /usr/x86_64-w64-mingw32/bin/libstdc++-6.dll ./
	cp /usr/x86_64-w64-mingw32/bin/libgcc_s_seh-1.dll ./
	cp /usr/x86_64-w64-mingw32/bin/libwinpthread-1.dll ./
	cp /usr/x86_64-w64-mingw32/bin/zlib1.dll ./

