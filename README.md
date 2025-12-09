Para compilar, desde la carpeta del proyecto : 

      g++ minimal_agent.cpp -o minimal_agent \
          -I Arcade-Learning-Environment-0.6.1 \
          -L Arcade-Learning-Environment-0.6.1 \
          -lale -lz -lm -std=c++17

Ejecutar y jugar : 

      ./minimal_agent roms/supported/assault.bin
