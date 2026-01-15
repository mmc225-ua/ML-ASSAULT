#include <iostream>
#include <vector>
#include <fstream>
#include <sstream> // Necesario para stringstream
#include <string>
#include <cstring>
#include "RRNN.hpp"
using namespace std;

struct Persona
{
    int edad;
    int genero;     // 0 female, 1 male, 2 other
    int plataforma; // 0-4
    int tiempo_pantalla;
    int tiempo_sociales;
    int positivas;
    int negativas;
    double horas_suenyo;
    int actividad_fisica;
    int ansiedad;
    int estres;
    int humor;
    int estado; // 0, 1, 2
};

void arreglodato(Persona &per, string palabra, int tipo)
{

    switch (tipo)
    {
    case 0:
        per.edad = stoi(palabra);
        // cout <<  per.edad;
        break;
    case 1: { 
        int gen = 0;
        if (palabra == "Female")
        {
            gen = 0;
        }
        else if (palabra == "Male")
        {
            gen = 1;
        }
        else if (palabra == "Other")
        {
            gen = 2;
        }
        else
        {
            gen = 2;
        }

        per.genero = gen;
        break;
    }
    case 2: {

        int plat = 0;

        if (palabra == "Facebook")
        {
            plat = 0;
        }
        else if (palabra == "TikTok")
        {
            plat = 1;
        }
        else if (palabra == "YouTube")
        {
            plat = 2;
        }
        else if (palabra == "WhatsApp")
        {
            plat = 3;
        }
        else if (palabra == "Snapchat")
        {
            plat = 4;
        }
        else
        {
            plat = 4;
        }
        per.plataforma = plat;
        break;
    }
    case 3:
        per.tiempo_pantalla = stoi(palabra);
        break;
    case 4:
        per.tiempo_sociales = stoi(palabra);
        break;
    case 5:
        per.positivas = stoi(palabra);
        break;
    case 6:
        per.negativas = stoi(palabra);
        break;
    case 7:
        per.horas_suenyo = stod(palabra);
        break;
    case 8:
        per.actividad_fisica = stoi(palabra);
        break;
    case 9:
        per.ansiedad = stoi(palabra);
        break;
    case 10:
        per.estres = stoi(palabra);
        break;
    case 11:
        per.humor = stoi(palabra);
        break;
    case 12:{

        int es = 0;
        //cout << "comparando " << palabra << "?" << endl;
        if (palabra == "Stressed")
        {
            es = 0;
        }
        else if (palabra == "Healthy")
        {
            es = 1;
        }
        else if (palabra == "At_Risk")
        {
            es = 2;
        }
        per.estado = es;

        break;
    }
    }
    
}

int main()
{
    vector<Persona> personas;

    fstream file;

    file.open("mental_health_social_media_dataset.csv", ios::in);

    if (file.is_open())
    {
        // voy a asumir que en ram_fichero

        string linea, word;

        getline(file, linea); // la primera es de las columnas no necesarias

        while (getline(file, linea))
        {

            stringstream s(linea);

            // cout << linea << endl << endl;

            // me quedo con los datos 1, 3, 4, ....

            // o sea obvio 0 y 2

            string palabra;
            int numerodato = 0;

            int numeroreal = 0;

            Persona per;
            for (int i = 0; i < linea.length(); i++)
            {

                if (linea[i] != ',')
                    palabra += linea[i];

                else
                {

                    // como esto no es editable y lo voy a dejar asi no voy a hacer un vector de parametros evitables (simplicidad)
                    if (!(numerodato == 2 || numerodato == 0))
                    {
                       // cout << numeroreal << " " << palabra << "|" << endl;

                        arreglodato(per, palabra, numeroreal);

                        numeroreal++;
                    }

                    numerodato++;
                    palabra = "";
                }
            }
            palabra.pop_back();
            
            arreglodato(per, palabra, numeroreal);
            
            

        
            personas.push_back(per);
        }
    }

    vector<vector<double>> input_personas;
    vector<vector<double>> output_personas;
    
    // ahora hay que pasarlo todo a un vector de doubles porque hace calculos con los pesos de la red neuronal, que son doubles aleatorios




    // SEPARACION POR INPUT Y OUTPUT 

    // no es buena idea predecir el estado mental porque es stressed en la mayoria, por lo que aprendería a decir stressed y acertar
    // ia el 90% de los casos


    for(int i = 0; i < personas.size(); i++){

        vector<double> p_d; // persona double
        vector<double> o_d; // output

        double edad = double(personas[i].edad);
        double gen = double(personas[i].genero);
        double plat = double(personas[i].plataforma);
        double t_p = double(personas[i].tiempo_pantalla);
        double t_s = double(personas[i].tiempo_sociales);
        double pos = double(personas[i].positivas);
        double neg = double(personas[i].negativas);
        
        double h_s = double(personas[i].horas_suenyo); // 1 de salida
        
        double a_fisi = double(personas[i].actividad_fisica);
        double ans = double(personas[i].ansiedad);
        double estres = double(personas[i].estres);
        double humor = double(personas[i].humor);
        double estado = double(personas[i].estado);

        // 12  de entrada

        p_d.push_back(edad/100);
        p_d.push_back(gen/100);
        p_d.push_back(plat/100);
        p_d.push_back(t_p/100);
        p_d.push_back(t_s/100);
        p_d.push_back(pos/100);
        p_d.push_back(neg/100);
        
        o_d.push_back(h_s/100);
        
        p_d.push_back(a_fisi/100);
        p_d.push_back(ans/100);
        p_d.push_back(estres/100);
        p_d.push_back(humor/100);
        //p_d.push_back(estado/100);

        input_personas.push_back(p_d);

        output_personas.push_back(o_d);
    }

    /* 
    for (size_t i = 0; i < input_personas.size(); i++) {
        cout << "Persona " << i << ": ";
        for (size_t j = 0; j < input_personas[i].size(); j++) {
            cout << input_personas[i][j];
            if (j < input_personas[i].size() - 1)
                cout << ", ";
        }
        cout << endl;
    }

    
    for (size_t i = 0; i < output_personas.size(); i++) {
        cout << "Persona " << i << ": ";
        for (size_t j = 0; j < output_personas[i].size(); j++) {
            cout << output_personas[i][j];
            if (j < output_personas[i].size() - 1)
                cout << ", ";
        }
        cout << endl;
    }*/


    // si probais con esto imprime 0,1,2 capas, la primera NO

    

    /*
    vector<double> test_persona {
    24.0/100.0,
    1.0/100.0,
    4.0/100.0,
    30.0/100.0,
    1.0/100.0,
    1.0/100.0,
    2.0/100.0,
    2.0/100.0,
    1.0/100.0,
    7.0/100.0,
    8.0/100.0,
    
    };


    cout << endl << endl << endl << endl << ("aqui ES LA PREDICCION") << endl << endl << endl << endl << endl;

    
    vector<double> prediccion_suenyo = red.forward(test_persona);



    cout << "ESTE ES UN EJEMPLO DE ENTRADA para ver que tal...." << endl;
    cout << "Reyansh Ghosh,35,1/1/2024,Male,Instagram,320,160,1,2,7.4,28,2,7,6,Stressed" << endl;
    cout << "DEBERIA DAR 7.4 aprox" << endl;
    cout << prediccion_suenyo[0] * 100 << endl;

    */

    vector<vector<double>> X_train, Y_train, X_test, Y_test;
    double porcentaje_train = 77;
    double porcentaje_test = 100.0 - porcentaje_train;
    
    double numero_train = input_personas.size() * (porcentaje_train / 100);
    double numero_test = input_personas.size() * (porcentaje_test / 100);

    cout << "filas entrenamiento " << numero_train << ", filas test " << numero_test << ": " << numero_train + numero_test << endl << endl;
    
    for (int i = 0; i < numero_train; i++){
        
        X_train.push_back(input_personas[i]);
        Y_train.push_back(output_personas[i]);
        //cout << i+1 << "," ;
    }

    cout << endl << endl << endl;
    for (int i = input_personas.size() - numero_test; i < input_personas.size(); i++){
        
        X_test.push_back(input_personas[i]);
        Y_test.push_back(output_personas[i]);
        //cout << i+1 << "," ;
    }




    RedBackPropagation red_suenyo({11, 10, 1});
    red_suenyo.tasa_aprendizaje = 0.1;
    red_suenyo.entrenar(X_train, Y_train, 5000);


    vector<vector<double>> prediccion_suenyo_test;

    cout << endl;
    for(int i = 0; i < numero_test; i++){
        prediccion_suenyo_test.push_back(red_suenyo.forward(X_test[i]));
        
        for(int j = 0; j < prediccion_suenyo_test.size(); j++){
            cout << "OBTENIDO " << prediccion_suenyo_test[j][0] << " FRENTE A " << Y_test[j][0] << endl;
        }
    }

   double errorPorcentualTotal = 0.0;
   for (int i = 0; i < numero_test; i++) {
        double error = fabs(prediccion_suenyo_test[i][0] - Y_test[i][0]) / Y_test[i][0];
        errorPorcentualTotal += error;
    }
    double MAPE = (errorPorcentualTotal / numero_test) * 100.0;
    double precision = 100.0 - MAPE;

    cout << "MAPE: " << MAPE << " precisión: " << precision << endl; 



    vector<double> test_persona_buenoshabitos {
    24.0/100.0,
    1.0/100.0,
    4.0/100.0,
    30.0/100.0,
    1.0/100.0,
    1.0/100.0,
    2.0/100.0,
    2.0/100.0,
    1.0/100.0,
    7.0/100.0,
    8.0/100.0,
    
    };

    vector<double> test_persona_maloshabitos {
    24.0/100.0, // age
    1.0/100.0, // gender
    4.0/100.0, // platf
    700.0/100.0, // minut movil
    600.0/100.0, // minut redes
    20.0/100.0, // neg
    1.0/100.0, // pos
    12.0/100.0, // actividad fisica
    7.0/100.0, // ansiedad
    6.0/100.0, // estres
    2.0/100.0, // mood
    };

    vector<double> suenyo_bueno = red_suenyo.forward(test_persona_buenoshabitos);
    vector<double> suenyo_malo = red_suenyo.forward(test_persona_maloshabitos);

    cout << "Sueño de una persona con buenos habitos de vida y en redes: " << suenyo_bueno[0] * 100 << " horas de sueño" << endl; 
    cout << "Sueño de una persona con malos habitos de vida y en redes: " << suenyo_malo[0] * 100 << " horas de sueño" << endl; 


    return 0;
}