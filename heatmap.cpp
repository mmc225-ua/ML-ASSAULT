#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>


class Node {
public:
	Node(){
		cell = -1;
		heat = -1;
	}
	Node(int c, int h) {
		cell = c;
		heat = h;
	}
	Node(const Node &n2) {
		cell = n2.cell;
		heat = n2.heat;
	}
	~Node(){}
	int cell;
	int heat;
	bool operator<(const Node& n2) {
		if (this->heat < n2.heat)
			return true;
		return false;
	}
	bool operator>(const Node& n2) {
		if (this->heat > n2.heat)
			return true;
		return false;
	}
	
};

void bubbleSort(std::vector<Node> &v) {
	int n = v.size();

	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < n - i - 1; j++) {
			if (v[j] < v[j + 1]) {
				Node aux(v[j]);
				v[j] = v[j + 1];
				v[j + 1] = aux;
			}
		}
	}
}

void printV(std::vector<std::string> v) {
	for (unsigned i = 0; i < v.size(); i++) {
		std::cout << v[i] << ",";
	}
	std::cout << "\n";
}

int main(int argc, char *argv[]) {
	std::fstream file;
	std::vector<std::string> actions;
	std::vector<int> heatmap(128, 0);
	std::vector<int> heatmap_left(128, 0);
	std::vector<int> heatmap_right(128, 0);
	std::vector<int> heatmap_upfire(128, 0);
	file.open("assault_run2.csv", std::ios::in);
	unsigned left_lines, right_lines, upfire_lines, lines;
	left_lines = right_lines = upfire_lines = lines = 0;
	if (file.is_open()) {
		std::vector<int> initial_ram(128, 0);
		std::string line, word, temp;
		//Cogemos la cabecera del fichero
		getline(file, line);
		
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
		//Ahora comparamos los valores del resto de las lineas
		while (file >> temp) {
			getline(file, line);
			std::stringstream ss(temp);
			contador = iram = 0;
			bool left, right, upfire;
			left = right = upfire = false;
			lines++;
			while (getline(ss, word, ',')) {
				if (contador == 1) {
					/*
					bool found = false;
					unsigned i = 0;
					while (i < actions.size() && !found) {
						if (actions[i] == word)
							found = true;
						i++;
					}
					if (!found)
						actions.emplace_back(word);
					*/
					if (word == "3") {
						right = true;
						right_lines++;
					}else if (word == "4") {
						left = true;
						left_lines++;
					}else if (word == "10"){
						upfire = true;
						upfire_lines++;
					}
				}
				if (contador >= 3) {
					iram = stoi(word);
					if (iram != initial_ram[contador-3]){
						heatmap[contador-3]++;
						if (right)
							heatmap_right[contador - 3]++;
						else if (left)
							heatmap_left[contador - 3]++;
						else if (upfire)
							heatmap_upfire[contador-3]++;
					}
				}
				contador++;
			}
		}
		file.close();
	} else {
		std::cerr << "ERROR: No se puedo abrir el csv\n";
	}
	std::vector<Node> v(128, Node());
	std::vector<Node> v_left(128, Node());
	std::vector<Node> v_right(128, Node());
	std::vector<Node> v_upfire(128, Node());
	for (unsigned i = 0; i < 128; i++) {
		Node n(i,heatmap[i]);
		v[i] = n;
		Node n_left(i,heatmap_left[i]);
		v_left[i] = n_left;
		Node n_right(i,heatmap_right[i]);
		v_right[i] = n_right;
		Node n_upfire(i,heatmap_upfire[i]);
		v_upfire[i] = n_upfire;
	}
	bubbleSort(v);
	bubbleSort(v_left);
	bubbleSort(v_right);
	bubbleSort(v_upfire);
	std::cout << "General\n" << lines << " lines\n";
	for (unsigned i = 0; i < v.size(); i++) {
		std::cout << "Ram[" << v[i].cell << "]: " << v[i].heat << "\t\t" << ((float)v[i].heat/(float)lines) * 100.0 << "%\n";
	}
	std::cout << "Izquierda\n" << left_lines << " lines\n";
	for (unsigned i = 0; i < v_left.size(); i++) {
		std::cout << "Ram[" << v_left[i].cell << "]: " << v_left[i].heat << "\t\t" << ((float)v_left[i].heat/(float)left_lines) * 100.0 << "%\n";
	}
	std::cout << "Derecha\n" << right_lines << " lines\n" ;
	for (unsigned i = 0; i < v_right.size(); i++) {
		std::cout << "Ram[" << v_right[i].cell << "]: " << v_right[i].heat << "\t\t" << ((float)v_right[i].heat/(float)right_lines) * 100.0 << "%\n";
	}
	std::cout << "Up fire\n" << upfire_lines << " lines\n";
	for (unsigned i = 0; i < v_upfire.size(); i++) {
		std::cout << "Ram[" << v_upfire[i].cell << "]: " << v_upfire[i].heat << "\t\t" << ((float)v_upfire[i].heat/(float)upfire_lines) * 100.0 << "%\n";
	}
}