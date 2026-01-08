#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

#include "RRNN.hpp"

int main()
{

    // voy a probar con un ejemplo muy MUY SENCILLO con (movimiento_enemigo, ataque_enemigo, movimiento_jugador, ataque_jugador)


    
    vector<vector<double>> enemigo = {
        {0.0, 0.0}, {0.0, 0.1}, {0.0, 0.2}, {0.0, 0.3}, {0.0, 0.4}, {0.0, 0.5}, {0.0, 0.6}, {0.0, 0.7}, {0.0, 0.8}, {0.0, 0.9}, {0.0, 1.0}, {0.1, 0.0}, {0.1, 0.1}, {0.1, 0.2}, {0.1, 0.3}, {0.1, 0.4}, {0.1, 0.5}, {0.1, 0.6}, {0.1, 0.7}, {0.1, 0.8}, {0.1, 0.9}, {0.1, 1.0}, {0.2, 0.0}, {0.2, 0.1}, {0.2, 0.2}, {0.2, 0.3}, {0.2, 0.4}, {0.2, 0.5}, {0.2, 0.6}, {0.2, 0.7}, {0.2, 0.8}, {0.2, 0.9}, {0.2, 1.0}, {0.3, 0.0}, {0.3, 0.1}, {0.3, 0.2}, {0.3, 0.3}, {0.3, 0.4}, {0.3, 0.5}, {0.3, 0.6}, {0.3, 0.7}, {0.3, 0.8}, {0.3, 0.9}, {0.3, 1.0}, {0.4, 0.0}, {0.4, 0.1}, {0.4, 0.2}, {0.4, 0.3}, {0.4, 0.4}, {0.4, 0.5}, {0.4, 0.6}, {0.4, 0.7}, {0.4, 0.8}, {0.4, 0.9}, {0.4, 1.0}, {0.5, 0.0}, {0.5, 0.1}, {0.5, 0.2}, {0.5, 0.3}, {0.5, 0.4}, {0.5, 0.5}, {0.5, 0.6}, {0.5, 0.7}, {0.5, 0.8}, {0.5, 0.9}, {0.5, 1.0}, {0.6, 0.0}, {0.6, 0.1}, {0.6, 0.2}, {0.6, 0.3}, {0.6, 0.4}, {0.6, 0.5}, {0.6, 0.6}, {0.6, 0.7}, {0.6, 0.8}, {0.6, 0.9}, {0.6, 1.0}, {0.7, 0.0}, {0.7, 0.1}, {0.7, 0.2}, {0.7, 0.3}, {0.7, 0.4}, {0.7, 0.5}, {0.7, 0.6}, {0.7, 0.7}, {0.7, 0.8}, {0.7, 0.9}, {0.7, 1.0}, {0.8, 0.0}, {0.8, 0.1}, {0.8, 0.2}, {0.8, 0.3}, {0.8, 0.4}, {0.8, 0.5}, {0.8, 0.6}, {0.8, 0.7}, {0.8, 0.8}, {0.8, 0.9}, {0.8, 1.0}, {0.9, 0.0}, {0.9, 0.1}, {0.9, 0.2}, {0.9, 0.3}, {0.9, 0.4}, {0.9, 0.5}, {0.9, 0.6}, {0.9, 0.7}, {0.9, 0.8}, {0.9, 0.9}, {0.9, 1.0}, {1.0, 0.0}, {1.0, 0.1}, {1.0, 0.2}, {1.0, 0.3}, {1.0, 0.4}, {1.0, 0.5}, {1.0, 0.6}, {1.0, 0.7}, {1.0, 0.8}, {1.0, 0.9}, {1.0, 1.0}};

    vector<vector<double>> personaje;


    // que responda a una funcion a ver si capta el patron
    for (auto &v : personaje)
    {
        double y1 = (v[0] + v[1]) / 2.0;
        double y2 = (v[0] - v[1] + 1.0) / 2.0; 
        enemigo.push_back({y1, y2});
    }


    RedBackPropagation red({2, 6, 2});
    red.tasa_aprendizaje = 0.1; // a ver yo en dp usaba esta pero no se si aqui funcionara igual
    red.entrenar(enemigo, personaje, 6000); 

    // ahora la pruebo FORWARD PARA OBTENER UN RESULTADO A PARTIR DE UN ENEMIGO

    vector<double> test_enemigo = {0.6, 0.2};
    vector<double> prediccion_personaje = red.forward(test_enemigo);

    
    cout << "ENTRADA VALORES DE ENEMIGO: " << test_enemigo[0] << ", " << test_enemigo[1];
    


    // AJUSTES POR LA FUNCION DE ARRIBA.... NADA MAS
    double y1_pred = prediccion_personaje[0] * 2.0;
    double y2_pred = prediccion_personaje[1] * 2.0 - 1.0;
    double y1_real = test_enemigo[0] + test_enemigo[1];
    double y2_real = test_enemigo[0] - test_enemigo[1];
    
    
    cout << "Predicción de la red: y1=" << y1_pred << ", y2=" << y2_pred << endl;
    cout << "Valor real: " << y1_real << ", " << y2_real << endl;




    return 0;
}
