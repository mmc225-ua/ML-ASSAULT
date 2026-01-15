// esto define lo acordado en llamada que es la red neuronal
#ifndef __RRNN_HPP__
#define __RRNN_HPP__

#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

class Neurona
{

public:
    // como es una red que deberia ser dinamica voy a poner vector de pesos que puedan cambiar
    // para mantener el orden, si la neurona no es apuntada por la neurona correspondiente en orden, le ponemos 0
    // tamaño del vector de pesos sera el tamaño (numero de neuronas) de la capaa anterior
    vector<double> pesos;

    double bias;

    double salida;   // salida de la neurona
    double delta_bp; // delta del backpropagation que es el error

    // la salida de la neurona no seria necesario ponerla creo porque seria la entrada de la siguiente capa, o sea que si es la ultima capa, esa ya tiene una entrada y ninguna salida.

    Neurona(size_t tam_capa_anterior) : pesos(tam_capa_anterior, 0.0), bias(0.0), salida(0.0), delta_bp(0.0)
    {

        // partimos de que los pesos de una red se inicializan de forma aleatoria

        bias = (((double)rand()) / RAND_MAX) - 0.5;

        for (double &peso : pesos)
        {
            peso = (((double)rand()) / RAND_MAX) - 0.5;
        }
    }
};

class Capa
{

public:
    vector<Neurona> neuronas;
    // CONSTRUCTOR BASICO
    Capa(size_t num_neuronas, size_t tam_capa_anterior)
    {

        for (size_t i = 0; i < num_neuronas; ++i)
        {

            neuronas.emplace_back(tam_capa_anterior);
        }
    }
};

class RedBackPropagation
{
public:
    vector<Capa> capas;

    double tasa_aprendizaje = 0.1; // es lo que dimos en DP, lo vamos a ir ajustando

    // FUNC SIGMOIDEA Y LA DERIVADA

    double sigmoid(double n)
    {
        double resultado = 1.0 / (1.0 + (exp(n * -1)));

        return resultado;
    }

    double sigmoid_derivada(double n)
    {
        double resultado = n * (1.0 - n);

        return resultado;
    }

    // es cantidad de capas
    // o sea que si tenemos {2, 4, 3}
    // tenemos 2 capas de entrada
    // 4 capas ocultas

    // 3 salidas (acciones: moverse a izquierda/ moverse a la derecha / atacar DEL PERSONAJE

    RedBackPropagation(const vector<size_t> &arquitectura)
    {

        for (size_t i = 1; i < arquitectura.size(); ++i)
        {
            // el tamaño de la capa de entrada de la neurona de capa n tiene tamaño de la capa anterior
            capas.emplace_back(arquitectura[i], arquitectura[i - 1]);
        }
    }

    // hacia delante

    vector<double> forward(const vector<double> &entrada)
    {
        cout << endl;
        cout << "en forward " << endl;

        int num_capa = 0;

        // esto son los 11 parametros de las personas
        vector<double> activaciones = entrada;

        for (auto &capa : capas)
        {

            // VALORES QUE SALEN DE CADA CAPA
            vector<double> nuevas_activaciones;

            cout << endl << "CAPA " << num_capa << endl;
            int num_neurona = 0;

            // por cada neurona de la capa en la que estamos,
            for (auto &neurona : capa.neuronas)
            {
                // suma ponderada de las entradas y el bias

                // por cada neurona, SUMA es su bias y la multiplicacion de los valores de la entrada de la red * los pesos de su vector de entrada (o sea que la señala desde otras neuronas)
                double suma = neurona.bias;
                //cout << "   num neurona " << num_neurona << endl;

                // NUESTRO CONJUNTO DE DATS
                for (size_t i = 0; i < activaciones.size(); i++)
                {
                    // cout << "añadiendo " << neurona.pesos[i] << " * " << activaciones[i] << endl;

                    // activaciones tiene el tamaño de la capa anterior
                    suma += neurona.pesos[i] * activaciones[i];
                }

                // neurona.salida = sigmoid(suma);
                // cout << "       suma de la neurona " << suma << endl;

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

            cout << "vector de entrada MODIFICADO por neuronas " << endl;
            cout << "[";// (deberia ser mismo largo que numero de neuronas de la capa)
            for (int i = 0; i < nuevas_activaciones.size(); i++)
            {
                cout << nuevas_activaciones[i] << ",";
            }
            cout << "]" << endl;

            activaciones = nuevas_activaciones;

            num_capa++;





            bool es_salida = (&capa == &capas.back());
            if (es_salida){

                cout << "ESTO ES LA SALIDA ";
                for (int i = 0; i < nuevas_activaciones.size(); i++){
                    cout << nuevas_activaciones[i] << ",";
                }
                cout << endl << "SALIDA - OBJETIVO ES EL ERROR, objetivo es SUEÑO real" <<endl;
            }
        }


        cout << endl;

        return activaciones;
    }




    

    void backpropagation(const vector<double> &entrada, const vector<double> objetivo)
    {
        cout << endl << "BACKPROPAGATION" << endl;
        Capa &capa_salida = capas.back(); // salida.neuronas.size() ES EL TAMAÑO DE LA CAPA

        for (size_t i = 0; i < capa_salida.neuronas.size(); i++)
        {

            double c_salida_output = capa_salida.neuronas[i].salida;

            capa_salida.neuronas[i].delta_bp = (c_salida_output - objetivo[i]); //* sigmoid_derivada(c_salida_output);
        }

        // VOY HACIA ATRAS EN LAS CAPAS:

        for (int j = capas.size() - 2; j >= 0; j--)
        {

            for (size_t i = 0; i < capas[j].neuronas.size(); i++)
            {
                cout << "encontrar error en NEURONA " << i << " de la capa OCULTA " << j << endl;

                double suma = 0.0;

                // es la capa siguiente, la mas cercna a la salida
                for (auto &neurona_siguiente : capas[j + 1].neuronas)
                {

                    suma += neurona_siguiente.pesos[i] * neurona_siguiente.delta_bp;
                }

                double capasnuevas_salidas = capas[j].neuronas[i].salida;

                capas[j].neuronas[i].delta_bp = suma * sigmoid_derivada(capasnuevas_salidas);
            }
        }

        // ACTUALIZO PESOS

        vector<double> activaciones_previas = entrada;

        for (auto &capa : capas)
        {

            for (auto &neurona : capa.neuronas)
            {

                for (size_t i = 0; i < neurona.pesos.size(); i++)
                {

                    // GRADIENTE DESCENDENTE
                    neurona.pesos[i] -= neurona.delta_bp * tasa_aprendizaje * activaciones_previas[i];
                }

                // ESTIY EN NEURONA AHORA

                neurona.bias -= tasa_aprendizaje * neurona.delta_bp;
            }

            // limpio el vector de activaciones previas

            activaciones_previas.clear();
            for (auto &n : capa.neuronas)
            {

                activaciones_previas.push_back(n.salida);
            }
        }
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

    // se suponr que 1 capa oculta es suficiente para los problemas de regresión
    // en el print se ve 0 y 1 (capa de salida y oculta, la de entrada no es explicita)
    void entrenar(const vector<vector<double>> input, const vector<vector<double>> output, int epocas)
    {

        for (int i = 0; i < epocas; i++)
        {

            // va a haber la misma cantidad de samples de datos enemigo y de jugador porque por cada ACCION DE ENEMIGO HAY UNA REACCION DE JUGADOR
            for (size_t i = 0; i < input.size(); i++)
            {

                forward(input[i]);
                backpropagation(input[i], output[i]);
            }
        }
    }
};

#endif
