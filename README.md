Para compilar, desde la carpeta del proyecto : <br>
        g++ Arcade-Learning-Environment-0.6.1/minimal_agent.cpp -o minimal_agent \
  -I Arcade-Learning-Environment-0.6.1 \
  -L Arcade-Learning-Environment-0.6.1 \
  -lale -lz -lm -std=c++17 \
  $(sdl-config --cflags --libs) \
  -Wl,-rpath,$(pwd)/Arcade-Learning-Environment-0.6.1

Ejecutar y jugar : <br>
    ./minimal_agent ./Arcade-Learning-Environment-0.6.1/supported/assault.bin

