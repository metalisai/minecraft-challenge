g++ main.cpp application.cpp Renderer/OpenGL/renderer.cpp game.cpp Renderer/mesh.cpp Renderer/material.cpp transform.cpp camera.cpp Renderer/texture.cpp GameWorld/chunk.cpp GameWorld/world.cpp GameWorld/chunkmanager.cpp -lsfml-window -lsfml-system -lGLEW -lGL -ggdb -std=gnu++0x -o ../game.out

if [ $1 == 'run' ]
then
sh -c 'cd .. && ./game.out'
fi
