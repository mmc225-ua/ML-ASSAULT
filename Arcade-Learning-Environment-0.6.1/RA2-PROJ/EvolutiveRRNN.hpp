// esto define lo acordado en llamada que es la red neuronal
#ifndef __RRNN_HPP__
#define __RRNN_HPP__

#include <iostream>
#include <vector>
#include <cmath>
#include <bits/stdc++.h>
using namespace std;

class Neurona {

public:
    // como es una red que deberia ser dinamica voy a poner vector de pesos que puedan cambiar
    // para mantener el orden, si la neurona no es apuntada por la neurona correspondiente en orden, le ponemos 0
    // tamaño del vector de pesos sera el tamaño (numero de neuronas) de la capaa anterior
    vector<double> pesos;

    double bias;

    double salida;   // salida de la neurona
    double delta_bp; // delta del backpropagation que es el error

    // la salida de la neurona no seria necesario ponerla creo porque seria la entrada de la siguiente capa, o sea que si es la ultima capa, esa ya tiene una entrada y ninguna salida.
    Neurona(size_t tam_capa_anterior) : pesos(tam_capa_anterior, 0.0), bias(0.0), salida(0.0), delta_bp(0.0) {
        // partimos de que los pesos de una red se inicializan de forma aleatoria
        bias = (((double)rand()) / RAND_MAX) - 0.5;
/*
        default_random_engine gen;
        uniform_real_distribution<double> distribution(base_weight, base_weight + 1);
        bias = distribution(gen);
*/
        for (unsigned i = 0; i < pesos.size(); i++) {
            //pesos[i] = distribution(gen);

            pesos[i] = (((double)rand()) / RAND_MAX) - 0.5;
        }
    }
};

class Capa {

public:
    vector<Neurona> neuronas;
    // CONSTRUCTOR BASICO
    Capa(size_t num_neuronas, size_t tam_capa_anterior) {
        for (size_t i = 0; i < num_neuronas; ++i) {
            neuronas.emplace_back(tam_capa_anterior);
        }
    }
};

class EvolutiveRRNN {
public:
    vector<Capa> capas;

    double tasa_aprendizaje = 0.1; // es lo que dimos en DP, lo vamos a ir ajustando

    // FUNC SIGMOIDEA Y LA DERIVADA
    double sigmoid(double n) {
        double resultado = 1.0 / (1.0 + (exp(n * -1)));
        return resultado;
    }

    double sigmoid_derivada(double n) {
        double resultado = n * (1.0 - n);
        return resultado;
    }

    // es cantidad de capas
    // o sea que si tenemos {2, 4, 3}
    // tenemos 2 capas de entrada
    // 4 capas ocultas

    // 3 salidas (acciones: moverse a izquierda/ moverse a la derecha / atacar DEL PERSONAJE

    EvolutiveRRNN(const vector<size_t> &arquitectura) {
        srand(time(NULL));
        for (size_t i = 1; i < arquitectura.size(); ++i) {
            // LA ENTRADA NO PORQUE LAS CAPAS ENTRADA SOLO TIENEN DATOS NO NEURONAS PREVIAS
            capas.emplace_back(arquitectura[i], arquitectura[i - 1]);
        }
    }

    void mutate(int degree) {
        default_random_engine gen(random_device{}());
        uniform_real_distribution<double> d(1, 1);
        if (degree == 1) {
            uniform_real_distribution<double> distribution(-0.05, 0.05);
            d = distribution;
        }
        else if (degree == 2) {
            uniform_real_distribution<double> distribution(-0.1, 0.1);
            d = distribution;
        }
        else if (degree == 3) {
            uniform_real_distribution<double> distribution(-0.5, 0.5);
            d = distribution;
        } else {
            uniform_real_distribution<double> distribution(-1, 1);
            d = distribution;
        }
        for (unsigned i = 0; i < capas.size(); i++) {
            for (unsigned j = 0; j < capas[i].neuronas.size(); j++) {
                capas[i].neuronas[j].bias += d(gen);
                for(unsigned k = 0; k < capas[i].neuronas[j].pesos.size(); k++) {
                    capas[i].neuronas[j].pesos[k] += d(gen);
                }
            }
        }
    }

    // hacia delante
    vector<double> forward(const vector<double> &entrada)
    {

        int num_capa = 0;

        // esto son los 11 parametros de las personas
        vector<double> activaciones = entrada;

        for (auto &capa : capas)
        {

            // VALORES QUE SALEN DE CADA CAPA
            vector<double> nuevas_activaciones;

            int num_neurona = 0;

            // por cada neurona de la capa en la que estamos,
            for (auto &neurona : capa.neuronas)
            {
                // suma ponderada de las entradas y el bias

                // por cada neurona, SUMA es su bias y la multiplicacion de los valores de la entrada de la red * los pesos de su vector de entrada (o sea que la señala desde otras neuronas)
                double suma = neurona.bias;

                // NUESTRO CONJUNTO DE DATS
                for (size_t i = 0; i < activaciones.size(); i++)
                {
                    // activaciones tiene el tamaño de la capa anterior
                    suma += neurona.pesos[i] * activaciones[i];
                }


                // SI ES LA CAPA DE SALIDA, me quedo son la suma como salida
                // SI NO, SE VA A SIGMOID
                bool es_salida = (&capa == &capas.back());
                neurona.salida = es_salida ? suma : sigmoid(suma);

                nuevas_activaciones.push_back(neurona.salida);

                // si es una capa mas, pasa a ser el vector de entrada actualizao

                // no hay problema porque el vector es tamaño igual al tamaño de la capa
                // salida de la capa es de tamaño de la capa

                num_neurona++;
            }

            activaciones = nuevas_activaciones;

            num_capa++;
        }

        return activaciones;
    }




    // EL ENTRENAMIENTO SE HACE CON TODO Y LA ULTIMA CAPA ES LA QUE DICE LO QUE SE HACE, PERO HAY QUE DARLE TODO PARA ENTRENARLA (activaciones)
    // EL PREDICTOR DIRA EN BASE AL MOVIMIENTO DEL ENEMIGO, LO QUE EL JUGADOR DEBE HACER
    // como la estructura sera asi 
    /*
        {
            [3, 4, 5, ...., 5],
            [3, 4, 5, ...., 5],
            [3, 4, 5, ...., 5]
        
        }
    y aun no sabemos los datos que vamos a incluir si o si, voy a hacer los dos vectores dinamicos
    */
    // EL CONJUNTO DE APRENDIZAJE TIENE INPUT Y OUTPUT: input es el movimiento del enemigo y output el movimiento del jugador PERO AMBOS SON EL 'INPUT' PARA ENTRENAR LA RED
    // para el preductor, la idea es que el input sea datos del enemigo y output sea datos del personaje
};

#endif
