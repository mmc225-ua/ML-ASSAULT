#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

#include "BP_RRNN.hpp"

int main()
{

    // voy a probar con un ejemplo muy MUY SENCILLO con (movimiento_enemigo, ataque_enemigo, movimiento_jugador, ataque_jugador)


    
    vector<vector<double>> input = {
        {0.0, 0.0}, {0.0, 0.1}, {0.0, 0.2}, {0.0, 0.3}, {0.0, 0.4}, {0.0, 0.5}, {0.0, 0.6}, {0.0, 0.7}, {0.0, 0.8}, {0.0, 0.9}, {0.0, 1.0}, {0.1, 0.0}, {0.1, 0.1}, {0.1, 0.2}, {0.1, 0.3}, {0.1, 0.4}, {0.1, 0.5}, {0.1, 0.6}, {0.1, 0.7}, {0.1, 0.8}, {0.1, 0.9}, {0.1, 1.0}, {0.2, 0.0}, {0.2, 0.1}, {0.2, 0.2}, {0.2, 0.3}, {0.2, 0.4}, {0.2, 0.5}, {0.2, 0.6}, {0.2, 0.7}, {0.2, 0.8}, {0.2, 0.9}, {0.2, 1.0}, {0.3, 0.0}, {0.3, 0.1}, {0.3, 0.2}, {0.3, 0.3}, {0.3, 0.4}, {0.3, 0.5}, {0.3, 0.6}, {0.3, 0.7}, {0.3, 0.8}, {0.3, 0.9}, {0.3, 1.0}, {0.4, 0.0}, {0.4, 0.1}, {0.4, 0.2}, {0.4, 0.3}, {0.4, 0.4}, {0.4, 0.5}, {0.4, 0.6}, {0.4, 0.7}, {0.4, 0.8}, {0.4, 0.9}, {0.4, 1.0}, {0.5, 0.0}, {0.5, 0.1}, {0.5, 0.2}, {0.5, 0.3}, {0.5, 0.4}, {0.5, 0.5}, {0.5, 0.6}, {0.5, 0.7}, {0.5, 0.8}, {0.5, 0.9}, {0.5, 1.0}, {0.6, 0.0}, {0.6, 0.1}, {0.6, 0.2}, {0.6, 0.3}, {0.6, 0.4}, {0.6, 0.5}, {0.6, 0.6}, {0.6, 0.7}, {0.6, 0.8}, {0.6, 0.9}, {0.6, 1.0}, {0.7, 0.0}, {0.7, 0.1}, {0.7, 0.2}, {0.7, 0.3}, {0.7, 0.4}, {0.7, 0.5}, {0.7, 0.6}, {0.7, 0.7}, {0.7, 0.8}, {0.7, 0.9}, {0.7, 1.0}, {0.8, 0.0}, {0.8, 0.1}, {0.8, 0.2}, {0.8, 0.3}, {0.8, 0.4}, {0.8, 0.5}, {0.8, 0.6}, {0.8, 0.7}, {0.8, 0.8}, {0.8, 0.9}, {0.8, 1.0}, {0.9, 0.0}, {0.9, 0.1}, {0.9, 0.2}, {0.9, 0.3}, {0.9, 0.4}, {0.9, 0.5}, {0.9, 0.6}, {0.9, 0.7}, {0.9, 0.8}, {0.9, 0.9}, {0.9, 1.0}, {1.0, 0.0}, {1.0, 0.1}, {1.0, 0.2}, {1.0, 0.3}, {1.0, 0.4}, {1.0, 0.5}, {1.0, 0.6}, {1.0, 0.7}, {1.0, 0.8}, {1.0, 0.9}, {1.0, 1.0}};

    vector<vector<double>> output;


    // que responda a una funcion a ver si capta el patron
    for (auto &v : input)
    {
        double y1 = exp(v[0] + v[1]);
        double y2 = exp(v[0] - v[1]); 
        output.push_back({y1, y2});
    }

    char activ = ' ';
    cout << "Qué tipo de activación quieres? (s: sigmoid, r:relu, t:tanh) ";
    cin >> activ;

    if(activ == 's'){
        cout << "USANDO ACTIVACIÓN SIGMOID" << endl;
    }
    else if (activ == 'r'){
        cout << "USANDO ACTIVACIÓN RELU" << endl;
    }
    else if (activ == 't'){
        cout << "USANDO ACTIVACIÓN TANH" << endl;
    }
    else{
        cout << "SIN ACTIVACIÓN (LINEAL)" << endl;
    }


    RedBackPropagation red({2, 6, 2});
    red.tasa_aprendizaje = 0.1; // a ver yo en dp usaba esta pero no se si aqui funcionara igual
    red.entrenar(input, output, 6000, activ); 

    // ahora la pruebo FORWARD PARA OBTENER UN RESULTADO A PARTIR DE UN ENEMIGO

    vector<double> test_input = {0.6, 0.2};
    vector<double> prediccion_output = red.forward(test_input, activ);


    cout << "INPUT: " << test_input[0] << ", " << test_input[1];
    


    // AJUSTES POR LA FUNCION DE ARRIBA.... NADA MAS
    double y1_pred = prediccion_output[0];
    double y2_pred = prediccion_output[1];
    double y1_real = exp(test_input[0]+ test_input[1]);
    double y2_real = exp(test_input[0]- test_input[1]);
    
    
    cout << endl <<"Predicción de la red: y1=" << y1_pred << ", y2=" << y2_pred << endl;
    cout << "Valor real: " << y1_real << ", " << y2_real << endl;




    return 0;
}
