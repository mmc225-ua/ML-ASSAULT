#include <iostream>
#include <vector>
#include <fstream>
#include <sstream> // Necesario para stringstream
#include <string>
#include <cstring>
#include "RRNN.hpp"
#include <algorithm>  // para desordenar el vector y que pueda entrenarse con todas las categorias al separar datos
#include <random> 
using namespace std;

/*

    Iris-setosa
    Iris-versicolor
    Iris-virginica

*/

double arreglodato(string palabra, int tipo)
{
    double val = 0;

    switch (tipo)
    {
    case 0:
    case 1:
    case 2:
    case 3:

        //cout << "flotantes " << endl;
        val = 0;
        val = stof(palabra) ;
        val = val / 100;
        break;
    case 4: { 
        //cout << "clasificacion " << endl;
        val = 0;
        if (palabra == "Iris-setosa"){
           val = 0; 
        }
        else if(palabra == "Iris-versicolor"){
            val = 1;
        }
        else if(palabra == "Iris-virginica"){
            val = 2;
        }
        else{

            val = 0;
        }

        val = val / 100;
        
        break;
    }
    
    }

    return val;
    
}

int main()
{
    
    vector<vector<double>> flores;

    fstream file;

    file.open("Iris.csv", ios::in);

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

            vector<double> flor;
            for (int i = 0; i < linea.length(); i++)
            {

                if (linea[i] != ',')
                    palabra += linea[i];

                else
                {

                    // como esto no es editable y lo voy a dejar asi no voy a hacer un vector de parametros evitables (simplicidad)
                    if (!(numerodato == 0)) // el id da igual porque solo va en orden
                    {
                       cout << numeroreal << " " << palabra << "|" << endl;

                        double dato = arreglodato(palabra, numeroreal);
                        flor.push_back(dato);

                        numeroreal++;
                    }

                    numerodato++;
                    palabra = "";
                }
            }
            cout << palabra << endl << endl;
            
            
            double dato = arreglodato(palabra, numeroreal);

            flor.push_back(dato);

            flores.push_back(flor);
            
            

        
            //personas.push_back(per);
        }
    }


    random_device rd;
    mt19937 gen(rd());
    shuffle(flores.begin(), flores.end(), gen);



    vector<vector<double>> flores_input;
    vector<vector<double>> flores_output;

    for(int i = 0; i < flores.size(); i++){

        int numvalor = 0;
        vector<double> input;
        vector<double> output;
        for(int j = 0; j < flores[i].size(); j++){

            if (numvalor == 0 || numvalor == 1 || numvalor == 2 || numvalor == 3){
                input.push_back(flores[i][j]);
            }
            else{
                output.push_back(flores[i][j]);
            }
            numvalor++;

        }

        flores_input.push_back(input);
        flores_output.push_back(output);
    }

    /*
    for (size_t i = 0; i < flores_input.size(); i++)
    {
        cout << "FLOR " << i << ": ";

        for (size_t j = 0; j < flores_input[i].size(); j++)
        {
            cout << flores_input[i][j] << " ";
        }

        cout << endl;
    }


    for (size_t i = 0; i < flores_output.size(); i++)
    {
        cout << "FLOR " << i << ": ";

        for (size_t j = 0; j < flores_output[i].size(); j++)
        {
            cout << flores_output[i][j] << " ";
        }

        cout << endl;
    }*/




    vector<vector<double>> X_train, Y_train, X_test, Y_test;
    double porcentaje_train = 70;
    double porcentaje_test = 100.0 - porcentaje_train;
    
    double numero_train = flores_input.size() * (porcentaje_train / 100);
    double numero_test = flores_input.size() * (porcentaje_test / 100);

    cout << "filas entrenamiento " << numero_train << ", filas test " << numero_test << ": " << numero_train + numero_test << endl << endl;
    
    for (int i = 0; i < numero_train; i++){
        
        X_train.push_back(flores_input[i]);
        Y_train.push_back(flores_output[i]);
        //cout << i+1 << "," ;
    }

    cout << endl << endl << endl;
    for (int i = flores_input.size() - numero_test; i < flores_input.size(); i++){
        
        X_test.push_back(flores_input[i]);
        Y_test.push_back(flores_output[i]);
        //cout << i+1 << "," ;
    }



    // ENTRENAMIENTO
    RedBackPropagation red_iris({4, 3, 1});
    red_iris.tasa_aprendizaje = 0.2;
    red_iris.entrenar(X_train, Y_train, 10);




    vector<vector<double>> predic_cat;

    cout << endl;
    for(int i = 0; i < numero_test; i++){
        predic_cat.push_back(red_iris.forward(X_test[i]));
        
        for(int i = 0; i < predic_cat.size(); i++){
            cout << "OBTENIDO " << predic_cat[i][0] << " FRENTE A " << Y_test[i][0] << endl;
        }
    }
    
    return 0;
}