#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>  // Necesario para stringstream

using namespace std;

#ifndef ENTIDADES_HPP
#define ENTIDADES_HPP

struct Posicion {
    int x;
    int y;
};

struct Ataque {
    Posicion pos_ataque;
};

struct Personaje {
    Posicion pos_personaje;
    Ataque ataque_personaje;
};

struct Ovni {
    Posicion pos_ovni;
    Ataque ataque_ovni;
};

struct InputRed {
    vector<Ovni> ovnis;  // Solo declaración, sin inicializar aquí
    Personaje personaje;
    
    // Constructor para inicializar el vector con 3 ovnis
    InputRed() : ovnis(3) {}
};

#endif




void imprimirInputs(const vector<InputRed>& inputs) {
    cout << "=== IMPRIMIENDO TODOS LOS INPUTS ===" << endl;
    
    for (size_t i = 0; i < inputs.size(); i++) {
        cout << "\n--- Input " << i << " ---" << endl;
        
        
        for (int j = 0; j < 3; j++) {
            
            cout << "Ovni " << j << ":" << endl;
            cout << "  Posicion: (" << inputs[i].ovnis[j].pos_ovni.x 
                 << ", " << inputs[i].ovnis[j].pos_ovni.y << ")" << endl;
            cout << "  Ataque: (" << inputs[i].ovnis[j].ataque_ovni.pos_ataque.x 
                 << ", " << inputs[i].ovnis[j].ataque_ovni.pos_ataque.y << ")" << endl;
        }
        
        
        cout << "Personaje:" << endl;
        cout << "  Posicion: (" << inputs[i].personaje.pos_personaje.x 
             << ", " << inputs[i].personaje.pos_personaje.y << ")" << endl;
        cout << "  Ataque: (" << inputs[i].personaje.ataque_personaje.pos_ataque.x 
             << ", " << inputs[i].personaje.ataque_personaje.pos_ataque.y << ")" << endl;
    }
    



    cout << "\nTotal de inputs: " << inputs.size() << endl;
}







int main() {
    
    // X_TRAIN: lo del enemigo
    // Y_TRAIN: lo del personaje

    // o1_pos_x, o1_pos_y, o1_ataq_x, o1_ataq_y, o2_pos_x, o2_pos_y, o2_ataq_x,
    // o2_ataq_y, o3_pos_x, o3_pos_y, o3_ataq_x, o3_ataq_y, per_x, per_y, per_ataq_x, per_ataq_y
    
    // hay 16 valores en el input de la red

    vector<InputRed> inputs;

    // EL ARCHIVO DE RAM FILTRADA SE LLAMARA 'ram_filtrada'

    fstream file;
    
    file.open("ram_filtrada.csv", ios::in);
    if(file.is_open()) {
        // voy a asumir que en ram_fichero
        string linea, word;

        
        

        while (getline(file, linea)) {

            stringstream s(linea);
            InputRed input;
            
            
            vector<int> valores;
            
            while (getline(s, word, ',')) {
                valores.push_back(stoi(word));
            }
        

            for (int i = 0; i < 3; i++) {
                int base_idx = i * 4; // 0, 4, 8
                
                input.ovnis[i].pos_ovni.x = valores[base_idx];
                input.ovnis[i].pos_ovni.y = valores[base_idx + 1];
                input.ovnis[i].ataque_ovni.pos_ataque.x = valores[base_idx + 2];
                input.ovnis[i].ataque_ovni.pos_ataque.y = valores[base_idx + 3];
            }
            
            
            input.personaje.pos_personaje.x = valores[12];
            input.personaje.pos_personaje.y = valores[13];
            input.personaje.ataque_personaje.pos_ataque.x = valores[14];
            input.personaje.ataque_personaje.pos_ataque.y = valores[15];
            
            inputs.push_back(input);
        }






        file.close();
    }


    imprimirInputs(inputs);
    
    return 0;
}