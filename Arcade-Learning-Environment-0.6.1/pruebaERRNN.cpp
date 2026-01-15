#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
using namespace std;

#include "EvolutiveRRNN.hpp"

const unsigned POBLATION_SIZE = 350;
const unsigned TOURNAMENT_ROUNDS = 1000;
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
void printVector(const vector<double> &v) {
    for (unsigned i = 0; i < v.size(); i++) {
        cout << v[i] << " ";
    }
    cout << "\n";
}

void next_gen(vector<RedBackPropagation> &p) {
    unsigned chosen = (POBLATION_SIZE * 5) / 100;
    unsigned rows = ((POBLATION_SIZE/chosen) - 1) / chosen;
    for (unsigned i = 0; i < chosen; i++) {
        for (unsigned j = 0; j < rows*chosen; j++) {
            RedBackPropagation r = p[i];
            r.mutate(1);
            p[(rows * i + 1) * chosen + j] = r;
        }
    }
    if (((POBLATION_SIZE/chosen) - 1) % chosen != 0) {
        RedBackPropagation r = p[1];
        for (unsigned i = (rows*(chosen) + 1) * chosen; i < POBLATION_SIZE - 5; i++) {
            r.mutate(3);
            p[i] = r;
        }
        for (unsigned i = POBLATION_SIZE - 6; i < POBLATION_SIZE; i++){
            r.mutate(4);
            p[i] = r;
        }
    }
}

void train(vector<RedBackPropagation>& p, const vector<vector<double>> &inputs, vector<double> outputs) {
    for (unsigned i = 0; i < TOURNAMENT_ROUNDS; i++) {
        double best_acc = 0;
        vector<double> vaccurracy;
        for (unsigned j = 0; j < POBLATION_SIZE; j++) {
            double accurracy = 0;
            for (unsigned k = 0; k < INPUTS_SIZE; k++ ) {
                vector<double> result = p[j].forward(inputs[k]);
                if (outputs[k] == 1) {
                    if (result[0] >= 1)
                        accurracy++;
                    else if (result[0] >= 0.5)
                        accurracy += 0.75;
                    else if (result[0] > 0.1)
                        accurracy += 0.5;
                    else if (result[0] < 0)
                        accurracy -= 0.25;
                } else if (outputs[k] == -1) {
                    if (result[0] <= -1)
                        accurracy++;
                    else if (result[0] <= -0.5)
                        accurracy += 0.75;
                    else if (result[0] < -0.1)
                        accurracy += 0.5;
                    else if (result[0] > 0)
                        accurracy -= 0.25;
                }
                if (i == TOURNAMENT_ROUNDS - 1 && j == 0)
                    cout << j << " " << result[0] << " -> " << outputs[k]  << " " << accurracy << "\n";
            }
            vaccurracy.emplace_back(accurracy);
            if (accurracy > best_acc) {
                best_acc = accurracy;
            }
        }
        bubbleSort(p,vaccurracy);
        cout << i << " " << best_acc << "\n";
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
                        output = -1.0;
                    else
                        output = 1.0;
                } else {
                    data = stof(word);
                    //AGE
                    if (contador == 0) {
                        data = (data - 29) / (77 - 29);
                    } else if (contador == 3) {
                        data = (data - 94) / (200 - 94);
                    } else if (contador == 4) {
                        data = (data - 126) / (564 - 126);
                    } else if (contador == 7) {
                        data = (data - 71) / (202 - 71);
                    }
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

void test( RedBackPropagation &p, const vector<vector<double>> &inputs, const vector<double> &outputs) {
    double correct_p, wrong_p, correct_a, wrong_a;
    correct_p = wrong_p = correct_a = wrong_a = 0;
    int full_size = 0;
    for (unsigned i = INPUTS_SIZE; i < inputs.size(); i++) {
        full_size++;
        std::vector<double> result = p.forward(inputs[i]);
        if (outputs[i] == 1) {
            if (result[0] >= 1)
                correct_p++;
            else
                wrong_p++;
        } else {
            if (result[0] <= -1)
                correct_a++;
            else
                wrong_a++;
        }
    }
    std::cout << "Total: "<< full_size << "\n";
    std::cout << "Presence guessed: "<< correct_p << " Presence missed: " << wrong_p << "\n";
    std::cout << "Absence guessed: "<< correct_a << " Absence missed: " << wrong_a << "\n";
    std::cout << "Guessed(%): " << ((correct_a + correct_p) / (inputs.size() - INPUTS_SIZE)) * 100 << "%\n";
}

int main() {
    vector<vector<double>> inputs;
    vector<double> outputs;
    parse(inputs, outputs);
    vector<RedBackPropagation> poblacion;
    for (unsigned i = 0; i < POBLATION_SIZE; i++) {
        RedBackPropagation red({inputs[0].size(), 6, 4, 1});
        red.tasa_aprendizaje = 0.1; // a ver yo en dp usaba esta pero no se si aqui funcionara igual
        poblacion.emplace_back(red);
    }
    train(poblacion, inputs, outputs);
    test(poblacion[0], inputs, outputs);

    return 0;
}
