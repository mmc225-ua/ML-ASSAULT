#include <iostream>
#include <vector>
#include <fstream>
#include <sstream> // Necesario para stringstream
#include <string>
#include <cstring>
#include "BP_RRNN.hpp"
using namespace std;



int conversor(string diagnostico){

    if (diagnostico == "Presence"){
       
        return 1;
    }
    else if (diagnostico == "Absence"){

        return 0;
    }
    else{

        return 0;
    }
}


int clasemayor(double n1, double n2){
    if(n1 >= n2){

        return 0; // ESTO ES PRIMERO;
    }
    else{
        return 1; // esta es SEGUNDO
    }
}

int main()
{

    vector<vector<double>> personas;
    fstream file;

    file.open("Heart_Disease_Prediction.csv", ios::in);



    if (file.is_open())
    {

        string linea, word;

        getline(file, linea); 

        while (getline(file, linea))
        {

            stringstream s(linea);

            

            string palabra;
            int numerodato = 0;

            int numeroreal = 0;
            vector<double> persona;
            
            for (int i = 0; i < linea.length(); i++)
            {

                if (linea[i] != ',')
                    palabra += linea[i];

                else
                {
                    double dato = stod(palabra) / 100;
                    
                    //cout << numeroreal << " " << dato << "|" << endl;

                    persona.push_back(dato);

                    

                    numeroreal++;
                    

                    numerodato++;
                    palabra = "";
                }
            }
            
            
            palabra.pop_back(); // tiene un \n al final
            
            
            int valorconvertido = conversor(palabra) ;

            if (valorconvertido == 0){

                persona.push_back(0.0);
                persona.push_back(1.0); // probabilidad de que salga ABSENCE
            }
            else if (valorconvertido == 1){
                persona.push_back(1.0); // probabilidad de que salga PRESENCE
                persona.push_back(0.0); 
            }
            else{
                cout << "no";
            }


            

            personas.push_back(persona);


            

            
        }
    }


    

    vector<vector<double>> input_personas;
    vector<vector<double>> output_personas;

    
    for(int i = 0; i < personas.size(); i++){

        int numparam = 0;

        vector<double> pred;
        vector<double> norm;
        for(int j = 0; j < personas[i].size(); j++){

            if (numparam == 13 || numparam == 14){
                // prediccion
                pred.push_back(personas[i][j]);
            }
            else{
                norm.push_back(personas[i][j]);

                
            }

            //cout << personas[i][j] << " ";

            numparam++;
        }
        //cout << endl;

        input_personas.push_back(norm);
        output_personas.push_back(pred);


    }



    vector<vector<double>> X_train, Y_train, X_test, Y_test;
    double porcentaje_train = 70;
    double porcentaje_test = 100.0 - porcentaje_train;
    
    double numero_train = input_personas.size() * (porcentaje_train / 100);
    double numero_test = input_personas.size() * (porcentaje_test / 100);

    cout << "filas entrenamiento " << numero_train << ", filas test " << numero_test << ": " << numero_train + numero_test << endl << endl;
    


    for(int i = 0; i < personas.size(); i++){

        for(int j = 0; j < input_personas[i].size(); j++){
            cout << input_personas[i][j];
        }
        cout << "   " ;
        for(int j = 0; j < output_personas[i].size(); j++){
            cout << output_personas[i][j];
        }

        cout << endl ;

    }


    for (int i = 0; i < numero_train; i++){
        
        X_train.push_back(input_personas[i]);
        Y_train.push_back(output_personas[i]);
        cout << i+1 << "," ;



    }

    cout << endl << endl << endl;


    for (int i = input_personas.size() - numero_test; i < input_personas.size(); i++){
        
        X_test.push_back(input_personas[i]);
        Y_test.push_back(output_personas[i]);
        cout << i+1 << "," ;
    }






    RedBackPropagation red_heart({13, 10, 2});
    red_heart.tasa_aprendizaje = 0.1;
    red_heart.entrenar(X_train, Y_train, 5000);


    vector<vector<double>> prediccion_heart_test;

    cout << endl;
    for(int i = 0; i < numero_test; i++){
        prediccion_heart_test.push_back(red_heart.forward(X_test[i]));
        
        int mayor = clasemayor(prediccion_heart_test[i][0], prediccion_heart_test[i][1]);
        cout << "OBTENIDO " << prediccion_heart_test[i][0] << ", "<< prediccion_heart_test[i][1] << " es la categoria " << mayor << "º " << " FRENTE A " << Y_test[i][0] << ", "<<  Y_test[i][1] << endl;
        
    }
    
    
    return 0;


}
