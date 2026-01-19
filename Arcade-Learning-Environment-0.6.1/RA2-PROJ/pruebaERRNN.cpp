#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
using namespace std;

#include "EvolutiveRRNN.hpp"

const unsigned POBLATION_SIZE = 300;
const unsigned TOURNAMENT_ROUNDS = 100;
const unsigned INPUTS_SIZE = 1500;

void bubbleSort(vector<EvolutiveRRNN> &p, vector<double> &performance) {
    int n = p.size();

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (performance[j] < performance[j + 1]) {
                swap(performance[j], performance[j + 1]);
                EvolutiveRRNN aux(p[j]);
                p[j] = p [j+1];
                p[j+1] = aux;
            }
        }
    }
}

void next_gen(vector<EvolutiveRRNN> &p) {
    unsigned chosen = (POBLATION_SIZE * 10) / 100;
    unsigned rows = ((POBLATION_SIZE/chosen) - 1) / chosen;
    for (unsigned i = 0; i < chosen; i++) {
        for (unsigned j = 0; j < rows*chosen; j++) {
            EvolutiveRRNN r = p[i];
            r.mutate(2);
            p[(rows * i + 1) * chosen + j] = r;
        }
    }
    if (((POBLATION_SIZE/chosen) - 1) % chosen != 0) {
        EvolutiveRRNN r = p[1];
        for (unsigned i = (rows*(chosen) + 1) * chosen; i < POBLATION_SIZE - chosen; i++) {
            r.mutate(3);
            p[i] = r;
        }
        for (unsigned i = POBLATION_SIZE - chosen; i < POBLATION_SIZE; i++){
            r.mutate(4);
            p[i] = r;
        }
    }
}

double fitness_pred(const vector<double>& result, const vector<double>& output) {
    double accurracy = 0;
    for (unsigned i = 0; i < result.size(); i++){
        double aux = result[i] - output[i];
        if (aux <= 0.01 && aux >= -0.01)
            accurracy++;
        else if (aux <= 0.1 && aux >= -0.1)
            accurracy += 0.75;
        else if (aux <= 0.5 && aux >= -0.5)
            accurracy += 0.5;
        else
            accurracy -= 0.25;
    }
    return accurracy / result.size();
}

double fitness_classs(const vector<double>& result, const vector<double>& output) {
    double accurracy = 0;
    for (unsigned i = 0; i < result.size(); i++) {
        if (output[i] == 1) {
            if (result[i] >= 1)
                accurracy++;
            else if (result[i] >= 0.5)
                accurracy += 0.75;
            else if (result[i] > 0.1)
                accurracy += 0.5;
            else if (result[i] < 0)
                accurracy -= 0.25;
        } else {
            if (result[i] <= -1)
                accurracy++;
            else if (result[i] <= -0.5)
                accurracy += 0.75;
            else if (result[i] < -0.1)
                accurracy += 0.5;
            else if (result[i] > 0)
                accurracy -= 0.25;
        }
    }

    return accurracy / result.size();
}

void train(vector<EvolutiveRRNN>& p, const vector<vector<double>> &inputs, vector<vector<double>> outputs) {
    for (unsigned i = 0; i < TOURNAMENT_ROUNDS; i++) {
        vector<double> vaccurracy;
        for (unsigned j = 0; j < POBLATION_SIZE; j++) {
            double accurracy = 0;
            for (unsigned k = 0; k < INPUTS_SIZE; k++ ) {
                vector<double> result = p[j].forward(inputs[k]);
                accurracy += fitness_pred(result, outputs[k]);
            }
            vaccurracy.emplace_back(accurracy);
        }
        bubbleSort(p,vaccurracy);
        next_gen(p);

    }
}

void parse_mental_health(vector<vector<double>> &inputs, vector<vector<double>> &outputs) {
    fstream file;
    file.open("mental_health_social_media_dataset.csv", ios::in);
    vector<double> row;
    vector<double> output;
    double data;
    if (file.is_open()) {
        string line, word, temp;
        getline(file, line);

        while(file >> temp) {
            stringstream ss(temp);
            unsigned contador = 0;
            while (getline(ss, word, ',')) {
                if (contador == 9) {
                    data = stof(word);
                    output.emplace_back(data);
                } else if (contador != 0 && contador != 14 && contador != 2) {
                    //AGE
                    if (contador == 4) {
                        if (word == "Instagram")
                            data = 0;
                        else if (word == "Snapchat")
                            data = 1;
                        else if (word == "Facebook")
                            data = 2;
                        else if (word == "WhatsApp")
                            data = 3;
                        else if (word == "TikTok")
                            data = 4;
                        else if (word == "YouTube")
                            data = 5;
                        else if (word == "Twitter")
                            data = 6;
                        data = data / 6;
                    } else if (contador == 3) {
                        if (word == "Male")
                            data = 0;
                        else if (word == "Female")
                            data = 0.5;
                        else
                            data = 1;
                    } else {
                        data = stof(word);
                        if (contador == 1) {
                            data = (data - 13) / (69 - 13);
                        } else if (contador == 5) {
                            data = (data - 140) / (520 - 140);
                        } else if (contador == 6) { // ver cuantas hay
                            data = (data - 35) / (338 - 35);
                        } else if (contador == 7) {
                            data = data / 2;
                        } else if (contador == 8) {
                            data = data / 4;
                        } else if (contador == 10) {
                            data = (data - 8) / (46 - 8);
                        } else if (contador == 11) {
                            data = (data - 1) / (4 - 1);
                        } else if (contador == 12) {
                            data = (data - 5) / (9 - 5);
                        } else if (contador == 13) {
                            data = (data - 13) / (47 - 13);
                        }
                    }
                    row.emplace_back(data);
                } 
                contador++;
            }
            if (contador != 1){
                inputs.emplace_back(row);
                outputs.emplace_back(output);
            }
            row.clear();
        }
        file.close();
    }
}

void parse_iris(vector<vector<double>> &inputs, vector<vector<double>> &outputs) {
    fstream file;
    file.open("Iris_Shuffled.csv", ios::in);
    vector<double> row;
    vector<double> output;
    double data;
    if (file.is_open()) {
        string line, word, temp;
        getline(file, line);

        while(file >> temp) {
            stringstream ss(temp);
            unsigned contador = 0;
            while (getline(ss, word, ',')) {
                if (contador == 5) {
                    if (word == "Iris-setosa")
                        output = {1, -1, -1};
                    else if (word == "Iris-versicolor")
                        output = {-1, 1, -1};
                    else
                        output = {-1, -1, 1};
                } else if (contador != 0) {
                    data = stof(word);
                    if (contador == 1) {
                        data = (data - 4.3) / (7.9 - 4.3);
                    } else if (contador == 2) {
                        data = (data - 2) / (4.4 - 2);
                    } else if (contador == 3) { // ver cuantas hay
                        data = (data - 1) / (6.9 - 1);
                    } else if (contador == 4) {
                        data = (data - 0.1) / (2.5 - 0.1);
                    }
                    row.emplace_back(data);
                } 
                contador++;
            }
            inputs.emplace_back(row);
            outputs.emplace_back(output);
            row.clear();
        }
        file.close();
    }
}

void parse_heart_disease(vector<vector<double>> &inputs, vector<vector<double>> &outputs) {
    fstream file;
    file.open("Heart_Disease_Prediction.csv", ios::in);
    if (file.is_open()) {
        string line, word, temp;
        getline(file, line);

        vector<double> row;
        vector<double> output;
        while(file >> temp) {
            stringstream ss(temp);
            unsigned contador = 0;
            while (getline(ss, word, ',')) {
                double data;
                if (contador == 13) {
                    if (word == "Absence")
                        output = {-1, 1};
                    else
                        output = {1, -1};
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


void test_class( EvolutiveRRNN &p, const vector<vector<double>> &inputs, const vector<vector<double>> &outputs) {
    vector<int> nclass(outputs[0].size(), 0);
    //Vector de m x m, donde m es el número de clases
    vector<vector<int>> difussion_map(outputs[0].size(),vector<int>(outputs[0].size(),0)); 
    for (unsigned i = INPUTS_SIZE; i < inputs.size(); i++) {
        std::vector<double> result = p.forward(inputs[i]);
        for (unsigned j = 0; j < outputs[i].size(); j++) {
            if (outputs[i][j] == 1)
                nclass[j]++;
            for (unsigned k = 0; k < result.size(); k++) {
                if (outputs[i][j] == 1 && result[k] >= 1) {
                    difussion_map[j][k]++;
                }
            }
        }
    }
    for (unsigned i = 0; i < difussion_map.size(); i++) {
        cout << "CLASE " << i << ":";
        for (unsigned j = 0; j < difussion_map[i].size(); j++) {
            cout << "\t" << difussion_map[i][j];
        }
        cout << "\n";
    }
    cout << "\t";
    for (unsigned i = 0; i < difussion_map.size(); i++) {
        cout << "\tCLASE " << i;    
    }
    cout << "\n";
    
    double total_correct, total_number;
    total_correct = total_number = 0.0;
    cout << "DIFMAP: " << difussion_map.size() << "\n";
     for (unsigned i = 0; i < difussion_map.size(); i++) {
        double recall = ((double)difussion_map[i][i] / nclass[i]) * 100;
        cout << "Recall Class" << i << ": " << recall << " %\n";
        total_correct += difussion_map[i][i];
        total_number += nclass[i];
    }
    std::cout << "Total accurracy: " << (total_correct / total_number) * 100 << " %\n";
}

void test_pred( EvolutiveRRNN &p, const vector<vector<double>> &inputs, const vector<vector<double>> &outputs) {
    double mean_error = 0.0;
    for (unsigned i = INPUTS_SIZE; i < inputs.size(); i++) {
        std::vector<double> result = p.forward(inputs[i]);
        double aux = result[0] - outputs[i][0];
        if (aux <= 0)
            mean_error -= aux;
        else
            mean_error += aux;
    }
    cout << "Mean Error: " << (mean_error / (double)(inputs.size() - INPUTS_SIZE)) << "\n";
}

int main() {
    vector<vector<double>> inputs;
    vector<vector<double>> outputs;
    parse_mental_health(inputs, outputs);

    vector<EvolutiveRRNN> poblacion;
    for (unsigned i = 0; i < POBLATION_SIZE; i++) {
        EvolutiveRRNN red({inputs[0].size(), 15, 10, 5, outputs[0].size()});
        red.tasa_aprendizaje = 0.1; // a ver yo en dp usaba esta pero no se si aqui funcionara igual
        poblacion.emplace_back(red);
    }
    
    train(poblacion, inputs, outputs);
    test_pred(poblacion[0], inputs, outputs);
    return 0;
}
