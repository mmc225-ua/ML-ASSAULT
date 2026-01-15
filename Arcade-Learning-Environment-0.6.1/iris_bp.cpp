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

       
        
        break;
    }
    
    }

    return val;
    
}


int mayor_valor(double primero, double segundo, double tercero){

    if(primero >= segundo && primero >= tercero){
        return 0;
    }
    else if(segundo >= tercero){

        return 1;
    }
    else{

        return 2;
    }
}


struct MConfusion{

    // x_mal_y ; clase x que fue predecida como y
    int c0 = 0;
    int c0_mal_c1= 0;
    int c0_mal_c2= 0;

    int c1= 0;
    int c1_mal_c0= 0;
    int c1_mal_c2= 0;

    int c2= 0;
    int c2_mal_c0= 0;
    int c2_mal_c1= 0;

};

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
            
            // aqui es de la clasificación, ONE HOT
            double dato = arreglodato(palabra, numeroreal);
            vector<double> onehot;

            cout << "PLANTA ES TIPO " << dato << endl;
            if (dato == 0){
                onehot = {
                    1, 0, 0
                };

                double res = 0;

                flor.push_back(1.0);
                flor.push_back(0.0);
                flor.push_back(0.0);
            }
            else if (dato == 1){
                onehot = {
                    0, 1, 0
                };

                
                flor.push_back(0.0);
                flor.push_back(1.0);
                flor.push_back(0.0);
            }
            else if(dato == 2){
                onehot = {
                    0, 0, 1
                };

                flor.push_back(0.0);
                flor.push_back(0.0);
                flor.push_back(1.0);
            }
            else{
                onehot = {
                    0, 0, 1
                };

                flor.push_back(0.0);
                flor.push_back(0.0);
                flor.push_back(1.0);
            }

            //flor.push_back(onehot);

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
    }




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
    RedBackPropagation red_iris({4, 6, 3});
    red_iris.tasa_aprendizaje = 0.1;
    red_iris.entrenar(X_train, Y_train, 5000);




    vector<vector<double>> predic_cat;

    vector<double> pred_norm;
    vector<double> res_norm;
    cout << endl;

    MConfusion matriz;

    for(int i = 0; i < numero_test; i++){
        predic_cat.push_back(red_iris.forward(X_test[i]));
        
        
            int mayor = mayor_valor(predic_cat[i][0], predic_cat[i][1], predic_cat[i][2]);

            int mayor_real = mayor_valor(Y_test[i][0], Y_test[i][1], Y_test[i][2]);
            // mayor = 0 (primero), 1 (segundo) , 2(tercero)

            cout << "PRED " << i << ": " << predic_cat[i][0] << ","<< predic_cat[i][1] << "," << predic_cat[i][2] << ": " <<mayor << " Y " << Y_test[i][0] << "," << Y_test[i][1] << "," << Y_test[i][2] << endl;
            pred_norm.push_back(mayor);


            if(mayor_real == mayor){
                if(mayor_real == 0){
                    matriz.c0++;
                }
                else if (mayor_real == 1){
                    matriz.c1++;
                }
                else if(mayor_real == 2){
                    matriz.c2++;
                }
                else{
                    matriz.c2++;
                }
            }
            else{

                if(mayor_real == 0 && mayor == 1){
                    matriz.c0_mal_c1++;
                }
                else if (mayor_real == 0 && mayor == 2){
                    matriz.c0_mal_c2++;
                }
                else if (mayor_real == 1 && mayor == 0){
                    matriz.c1_mal_c0++;
                }
                else if (mayor_real == 1 && mayor == 2){
                    matriz.c1_mal_c2++;
                }
                else if (mayor_real == 2 && mayor == 0){
                    matriz.c2_mal_c0++;
                }
                else if (mayor_real == 2 && mayor == 1){
                    matriz.c2_mal_c1++;
                }
                else{
                    cout << "no otro caso";
                }
            }

        
        }
    


    cout << "MATRIZ DE CONFUSION DE CLASIFICACIÓN DE IRIS" << endl;

    cout << "CLASE 0    " << matriz.c0 << "          " << matriz.c0_mal_c1 << "          " << matriz.c0_mal_c2 << endl;
    cout << "CLASE 1    " <<matriz.c1_mal_c0 << "          " << matriz.c1 << "          " << matriz.c1_mal_c2 << endl;
    cout << "CLASE 2    " <<matriz.c2_mal_c0 << "          " << matriz.c2_mal_c1 << "          " << matriz.c2 << endl;



    cout << "           CLASE 0    CLASE 1     CLASE 2     (clase ficticia)" << endl;
 
    
    
    return 0;
}