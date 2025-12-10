Para compilar, desde la carpeta del proyecto : 
    cd Arcade-Learning-Environment-0.6.1
    g++ minimal_agent.cpp libale.so -o minimal_agent -Wl,-rpath=.

Ejecutar y jugar : 
    cd Arcade-Learning-Environment-0.6.1
    ./minimal_agent supported/assault.bin
