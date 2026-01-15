#include <iostream>
#include <vector>
#include <fstream>
#include <sstream> // Necesario para stringstream
#include <string>
#include <cstring>
#include "RRNN.hpp"
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


    cout << "flores:";

    for (size_t i = 0; i < flores.size(); i++)
    {
        cout << "FLOR " << i << ": ";

        for (size_t j = 0; j < flores[i].size(); j++)
        {
            cout << flores[i][j] << " ";
        }

        cout << endl;
    }



    return 0;
}