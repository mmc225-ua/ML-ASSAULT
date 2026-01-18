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


int mayor_valor(double n1, double n2){
    if(n1 >= n2){

        return 0; // ESTO ES PRIMERO;
    }
    else{
        return 1; // esta es SEGUNDO
    }
}


struct MConf{
    int c1 = 0;
    int c1_mal_c0 = 0;
    int c0 = 0;
    int c0_mal_c1 = 0;

};

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

                persona.push_back(1.0);
                persona.push_back(0.0); // probabilidad de que salga ABSENCE
            }
            else if (valorconvertido == 1){
                persona.push_back(0.0); // probabilidad de que salga PRESENCE
                persona.push_back(1.0); 
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

    cout << "filas entrenamiento " << numero_train << ", filas test " << numero_test << ": " << numero_train + numero_test << endl;
    

/* 
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
*/

    for (int i = 0; i < numero_train; i++){
        
        X_train.push_back(input_personas[i]);
        Y_train.push_back(output_personas[i]);
        //cout << i+1 << "," ;



    }


    for (int i = input_personas.size() - numero_test; i < input_personas.size(); i++){
        
        X_test.push_back(input_personas[i]);
        Y_test.push_back(output_personas[i]);
        //cout << i+1 << "," ;
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


    RedBackPropagation red_heart({13, 10, 2});
    red_heart.tasa_aprendizaje = 0.01;
    double mse = red_heart.entrenar(X_train, Y_train, 10000, activ);


    vector<vector<double>> prediccion_heart_test;

    MConf m;

    cout << endl;
    for(int i = 0; i < numero_test; i++){
        prediccion_heart_test.push_back(red_heart.forward(X_test[i], activ));
        
        int mayor = mayor_valor(prediccion_heart_test[i][0], prediccion_heart_test[i][1]);
        cout << "OBTENIDO " << prediccion_heart_test[i][0] << ", "<< prediccion_heart_test[i][1] << " es la categoria " << mayor << "º " << " FRENTE A " << Y_test[i][0] << ", "<<  Y_test[i][1] ;
        int real = mayor_valor(Y_test[i][0], Y_test[i][1]);


        if(mayor == real){

            cout << " OK";


            if (mayor == 0){
                m.c0++;



            }
            else if(mayor == 1){
                m.c1++;


            }
            else{
                // cout << 
            }
        }
        else{
            cout << " NO";


            if(real == 0 && mayor == 1){
                m.c0_mal_c1++;
            }
            else if(real == 1 && mayor == 0){
                m.c1_mal_c0++;
            }
            else{
                // cout << 
            }
        }
        cout << endl;
        
    }





    double mse_validacion = 0.0;
    int N = Y_test.size();

    for(int i = 0; i < N; i++){
        double sample_error = 0.0;
        for(int j = 0; j < 2; j++){
            double diff = Y_test[i][j] - prediccion_heart_test[i][j];
            sample_error += diff * diff;
        }
        sample_error /= 2.0;  // hay 2 clases onehot
        mse_validacion += sample_error;
    }

    mse_validacion /= N; 





    double MAPE = 0.0;

    for(int i = 0; i < N; i++){


        int clase_real = mayor_valor(Y_test[i][0], Y_test[i][1]); 
        double pred_val = prediccion_heart_test[i][clase_real];
        double ape = abs(1.0 - pred_val); 

        //cout << "Comparando mio " << pred_val << " con " << clase_real << endl;
        
        MAPE += ape;
    }

    MAPE = (MAPE / N) * 100.0;




    int correctos = 0;


    for(int i = 0; i < N; i++){
        int clase_real = mayor_valor(Y_test[i][0], Y_test[i][1]);   // índice del 1
        int clase_pred = mayor_valor(prediccion_heart_test[i][0], prediccion_heart_test[i][1]); // índice del max

        if(clase_real == clase_pred) correctos++;
    }

    double precision = (double(correctos) / N) * 100.0;




    int tp[2] = {0, 0};   
    int fn[2] = {0, 0}; 

    for(int i = 0; i < N; i++){
        int clase_real = mayor_valor(Y_test[i][0], Y_test[i][1]);
        int clase_pred = mayor_valor(prediccion_heart_test[i][0], prediccion_heart_test[i][1]);

        if(clase_real == clase_pred){
            tp[clase_real]++;
        } else {
            fn[clase_real]++;
        }
    }

    for(int c = 0; c < 2; c++){
        double recall = 0.0;
        if(tp[c] + fn[c] > 0){
            recall = (double)tp[c] / (tp[c] + fn[c]);
        }
        cout << "Recall clase " << c << ": " << recall * 100 << "%" << endl;
    }



    cout << "MAPE: " << MAPE << " precisión: " << precision<< "%" << endl; 
    cout << "MSE de entrenamiento: " << mse  << endl;
    cout << "MSE de validacion: " << mse_validacion << endl << endl;



    cout << "MATRIZ DE CONFUSION DE ENFERMEDAD DEL CORAZÓN" << endl;

    cout << "CLASE 0    " << m.c0 << "          " << m.c0_mal_c1 << endl;
    cout << "CLASE 1    " <<m.c1_mal_c0 << "          " << m.c1  << endl;
   



    cout << "           CLASE 0    CLASE 1      (clase ficticia)" << endl;

    cout << endl << "ÍNDICE" << endl;
    cout << "CLASE O: Absence" << endl <<  "CLASE 1: Presence" << endl;

    
    
    return 0;


}
