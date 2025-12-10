Para compilar, desde la carpeta del proyecto : 
    cd Arcade-Learning-Environment-0.6.1
    g++ minimal_agent.cpp libale.so -o minimal_agent -Wl,-rpath=.

Ejecutar y jugar : 

      ./minimal_agent roms/supported/assault.bin
