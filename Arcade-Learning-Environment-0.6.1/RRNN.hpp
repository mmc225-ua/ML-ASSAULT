// esto define lo acordado en llamada que es la red neuronal
#ifndef __RRNN_HPP__
#define __RRNN_HPP__

#include <iostream>
#include <vector>
using namespace std;




class Neurona{

    // como es una red que deberia ser dinamica voy a poner vector de pesos que puedan cambiar
    // para mantener el orden, si la neurona no es apuntada por la neurona correspondiente en orden, le ponemos 0
    // tamaño del vector de pesos sera el tamaño (numero de neuronas) de la capaa anterior
    vector<double> pesos;

    double bias;

    // la salida de la neurona no seria necesario ponerla creo porque seria la entrada de la siguiente capa, o sea que si es la ultima capa, esa ya tiene una entrada y ninguna salida.

public:
    Neurona(size_t tam_capa_anterior)
        : pesos(tam_capa_anterior, 0.0), bias(0.0) {}
};


class Capa{

    vector<Neurona> neuronas;

public:
    // CONSTRUCTOR BASICO
    Capa(size_t num_neuronas, size_t tam_capa_anterior){
    
        for(size_t i = 0; i < num_neuronas; ++i){
            
            
            neuronas.emplace_back(tam_capa_anterior);
        }
    }
};


class Red{

    vector<Capa> capas;

public:
    Red(const vector<size_t>& arquitectura){
        
        
        for(size_t i = 1; i < arquitectura.size(); ++i){
        
        
            capas.emplace_back(arquitectura[i], arquitectura[i - 1]);
        
        }
    }
};

#endif
