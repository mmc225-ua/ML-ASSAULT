#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

void printV(std::vector<int> v) {
	for (int x : v) {
		std::cout << x << ",";
	}
	std::cout << "\n";
}

int main(int argc, char *argv[]) {
	std::fstream file;
	std::vector<int> heatmap(128, 0);
	file.open("assault_run1.csv", std::ios::in);
	if (file.is_open()) {
		std::vector<int> initial_ram(128, 0);
		std::string line, word, temp;
		//Cogemos la cabecera del fichero
		getline(file, line);
		//std::cout << line << "\n";
		
		//Primera linea de datos de la ram obtenida
		getline(file, line);
		std::stringstream s(line);
		int contador = 0, iram = 0;
		while (getline(s, word, ',')) {
			if (contador >= 3) {
				iram = stoi(word);
				initial_ram[contador-3] = iram;	
			}
			contador++;
		}
		//printV(initial_ram);
		//Ahora comparamos los valores del resto de las lineas
		while (file >> temp) {
			getline(file, line);
			std::stringstream ss(temp);
			contador = iram = 0;
			//std::cout << temp << "\n";
			while (getline(ss, word, ',')) {
				if (contador >= 3) {
					iram = stoi(word);
					if (iram != initial_ram[contador-3]){
						//std::cout << "Diff\n";
						heatmap[contador-3]++;
					}
				}
				contador++;
			}
		}
		file.close();
	} else {
		std::cerr << "ERROR: No se puedo abrir el csv\n";
	}
	int contador = 0;
	for (unsigned i = 0; i < heatmap.size(); i++) {
		if (heatmap[i] != 0) {
			contador++;
			std::cout << "Ram[" << i << "]: " << heatmap[i] << "\n";
		}
	}
	std::cout << "Número de posiciones: " << contador << "\n";
}