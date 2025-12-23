Para compilar, desde la carpeta del proyecto : <br>
        g++ Arcade-Learning-Environment-0.6.1/minimal_agent.cpp -o minimal_agent \
  -I Arcade-Learning-Environment-0.6.1 \
  -L Arcade-Learning-Environment-0.6.1 \
  -lale -lz -lm -std=c++17 \
  $(sdl-config --cflags --libs) \
  -Wl,-rpath,$(pwd)/Arcade-Learning-Environment-0.6.1

Ejecutar y jugar SIN GRABAR: <br>
    ./minimal_agent ./Arcade-Learning-Environment-0.6.1/supported/assault.bin

Ejecutar y jugar GRABANDO DATASET : <br>
                -> OJO ! -> cambiar el Nº de assault_runX.csv cada partida, si no se sobreescriben los datos
        ./minimal_agent ./Arcade-Learning-Environment-0.6.1/supported/assault.bin assault_run1.csv



Controles : 
        flecha izq -> mover izq
        flecha der -> mover der
        espacio -> disparo vertical
        izq + espacio -> disparo izq
        der + espacio -> disparo der


Tras jugar y guardar datos, podemos consultar datos del CSV así: 
        head -n 3 assault_run1.csv
        wc -l assault_run1.csv
