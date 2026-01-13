#include <iostream>
#include <vector>
#include <fstream>
#include <sstream> // Necesario para stringstream
#include <string>
#include <cstring>
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
            
            

            cout << per.edad << ", "
                 << per.genero << ", "
                 << per.plataforma << ", "
                 << per.tiempo_pantalla << ", "
                 << per.tiempo_sociales << ", "
                 << per.positivas << ", "
                 << per.negativas << ", "
                 << per.horas_suenyo << ", "
                 << per.actividad_fisica << ", "
                 << per.ansiedad << ", "
                 << per.estres << ", "
                 << per.humor << ", "
                 << per.estado << endl;
            personas.push_back(per);
        }
    }

    return 0;
}