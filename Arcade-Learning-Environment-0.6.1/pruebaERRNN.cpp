#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
using namespace std;

#include "EvolutiveRRNN.hpp"

const unsigned POBLATION_SIZE = 100;
const unsigned TOURNAMENT_ROUNDS = 25;
const unsigned INPUTS_SIZE = 81;

void bubbleSort(vector<RedBackPropagation> &p, vector<double> &performance) {
    int n = p.size();

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (performance[j] < performance[j + 1]) {
                swap(performance[j], performance[j + 1]);
                RedBackPropagation aux(p[j]);
                p[j] = p [j+1];
                p[j+1] = aux;
            }
        }
    }
}

void next_gen(vector<RedBackPropagation> &p) {
    for (unsigned i = 0; i < POBLATION_SIZE/10; i++) {
        for (unsigned j = 0; j < 5; j++) {
            p[(i+1)*5 + j].mutate(j+1);
        }
    }
}

void train(vector<RedBackPropagation>& p, const vector<vector<double>> &inputs, vector<double> outputs) {
    for (unsigned i = 0; i < TOURNAMENT_ROUNDS; i++) {
        int best_net = 0;
        int best_acc = 0;
        vector<double> vaccurracy;
        for (unsigned j = 0; j < POBLATION_SIZE; j++) {
            int accurracy = 0;
            for (unsigned k = 0; k < INPUTS_SIZE; k++ ) {
                vector<double> result = p[j].forward(inputs[k]);
                if (result[0] >= 0.5 && outputs[k] == 1) {
                    accurracy++;
                } else if (result[0] < 0.5 && outputs[k] == 0) {
                    accurracy++;
                }
                cout << i << " " << j << " " << k << " -> " << result[0] << "\n";
            }
            vaccurracy.emplace_back(accurracy);
            if (accurracy > best_acc) {
                best_acc = accurracy;
                best_net = j;
            }
        }
        bubbleSort(p,vaccurracy);
        cout << best_net << " " << best_acc << " " << vaccurracy[0] << "\n";
        next_gen(p);

    }

}

void parse(vector<vector<double>> &inputs, vector<double> &outputs) {
    fstream file;
    file.open("Heart_Disease_Prediction.csv", ios::in);
    if (file.is_open()) {
        string line, word, temp;
        getline(file, line);

        vector<double> row;
        double output;
        while(file >> temp) {
            stringstream ss(temp);
            unsigned contador = 0;
            while (getline(ss, word, ',')) {
                double data;
                if (contador == 13) {
                    if (word == "Absence")
                        output = 0.0;
                    else
                        output = 1.0;
                } else {
                    data = stof(word);
                }
                row.emplace_back(data);
                contador++;
            }
            inputs.emplace_back(row);
            outputs.emplace_back(output);
            row.clear();
        }
        file.close();
    }
}

int main() {
    vector<vector<double>> inputs;
    vector<double> outputs;
    parse(inputs, outputs);
    cout << inputs.size() << " " << inputs[0].size() <<"\n";
    vector<RedBackPropagation> poblacion;
    for (unsigned i = 0; i < POBLATION_SIZE; i++) {
        RedBackPropagation red({14, 6, 4, 2, 1});
        red.tasa_aprendizaje = 0.1; // a ver yo en dp usaba esta pero no se si aqui funcionara igual
        poblacion.emplace_back(red);
    }
    train(poblacion , inputs, outputs);


    return 0;
}
