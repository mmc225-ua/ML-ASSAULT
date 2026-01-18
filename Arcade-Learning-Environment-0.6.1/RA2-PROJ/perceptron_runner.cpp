#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <random>
#include <iomanip>
#include <limits>
#include <ctime>

using namespace std;
// Quita los espacios en blanco del principio y del final de un texto
static inline string trim(const string& s) {
    size_t a = 0;
    while (a < s.size() && isspace((unsigned char)s[a])) a++;
    size_t b = s.size();
    while (b > a && isspace((unsigned char)s[b - 1])) b--;
    return s.substr(a, b - a);
}

//Divide un texto por comas y devuelve una lista de trozos ya limpios
static vector<string> split_by_comma(const string& s) {
    vector<string> out;
    string cur;
    for (char c : s) {
        if (c == ',') {
            cur = trim(cur);
            if (!cur.empty()) out.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    cur = trim(cur);
    if (!cur.empty()) out.push_back(cur);
    return out;
}

// CSV parser simple con soporte básico de comillas
static vector<string> parse_csv_line(const string& line) {
    vector<string> fields;
    string cur;
    bool in_quotes = false;

    for (size_t i = 0; i < line.size(); i++) {
        char c = line[i];
        if (c == '"') {
            // Doble comilla escapada ""
            if (in_quotes && i + 1 < line.size() && line[i + 1] == '"') {
                cur.push_back('"');
                i++;
            } else {
                in_quotes = !in_quotes;
            }
        } else if (c == ',' && !in_quotes) {
            fields.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    fields.push_back(cur);
    return fields;
}

//Intenta convertir un texto a número. Devuelve true si pudo y false si no y acaba el programa
static bool try_double(const string& s, double& out) {
    string t = trim(s);
    if (t.empty()) return false;
    char* endp = nullptr;
    out = strtod(t.c_str(), &endp);
    if (endp == t.c_str() || *endp != '\0') return false;
    return true;
}

//Convierte una fecha tipo  M/D/YYYY ó M/D/YY a día del año (0...366). Si falla, marca ok= false
static double parse_date_to_day_of_year(const string& s, bool& ok) {
    string t = trim(s);
    ok = false;
    if (t.empty()) return 0.0;

    vector<string> parts;
    {
        string cur;
        for (char c : t) {
            if (c == '/') {
                parts.push_back(cur);
                cur.clear();
            } else {
                cur.push_back(c);
            }
        }
        parts.push_back(cur);
    }

    if (parts.size() != 3) return 0.0;

    int m = 0, d = 0, y = 0;
    try {
        m = stoi(trim(parts[0]));
        d = stoi(trim(parts[1]));
        y = stoi(trim(parts[2]));
    } catch (...) {
        return 0.0;
    }

    if (y >= 0 && y <= 99) {
        if (y <= 68) y += 2000;
        else y += 1900;
    }

    std::tm tmv{};
    tmv.tm_year = y - 1900;
    tmv.tm_mon  = m - 1;
    tmv.tm_mday = d;
    tmv.tm_hour = 12; 
    tmv.tm_isdst = -1;

    time_t tt = mktime(&tmv);
    if (tt == (time_t)-1) return 0.0;

    // tm_yday: 0..365
    ok = true;
    return double(tmv.tm_yday + 1);
}


// Carga + encoding (mixto numérico/categórico/fecha)
struct DatasetMixedResult {
    vector<vector<double>> X;
    vector<string> y_str;
    vector<string> feature_names;
    map<string, vector<string>> cat_map; // col -> valores ordenados
};

//Lee el CSV completo y construye la matriz x (números + one-hot de categóricas + fechas convertidas) y el vector de etiquetas y_str.
//Crea también los nombres de las características y el mapa de categorías
static DatasetMixedResult load_dataset_mixed_types(
    const string& path,
    const string& target_col,
    const vector<string>& drop_cols_vec,
    const vector<string>& date_cols_vec,
    const vector<string>& categorical_cols_vec
) {
    set<string> drop_cols(drop_cols_vec.begin(), drop_cols_vec.end());
    set<string> date_cols(date_cols_vec.begin(), date_cols_vec.end());
    set<string> categorical_cols(categorical_cols_vec.begin(), categorical_cols_vec.end());

    ifstream fin(path.c_str());
    if (!fin) {
        throw runtime_error("No se puede abrir el CSV: " + path);
    }

    string header_line;
    if (!getline(fin, header_line)) {
        throw runtime_error("CSV vacio: " + path);
    }

    vector<string> headers = parse_csv_line(header_line);
    for (auto& h : headers) h = trim(h);

    int target_idx = -1;
    for (int i = 0; i < (int)headers.size(); i++) {
        if (headers[i] == target_col) {
            target_idx = i;
            break;
        }
    }
    if (target_idx < 0) {
        throw runtime_error("Columna objetivo '" + target_col + "' no existe en el CSV.");
    }

    // Guardamos por fila un map col->valor (ya parcialmente procesado)
    vector<map<string, string>> rows_feat_raw;
    vector<string> y_str;

    map<string, set<string>> cat_values;
    for (const auto& c : categorical_cols) cat_values[c] = {};

    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        vector<string> fields = parse_csv_line(line);
        // tolerancia: si faltan campos, rellenamos con ""
        if ((int)fields.size() < (int)headers.size()) fields.resize(headers.size(), "");

        string label = trim(fields[target_idx]);
        y_str.push_back(label);

        map<string, string> feat;

        for (int i = 0; i < (int)headers.size(); i++) {
            if (i == target_idx) continue;

            string col = headers[i];
            if (drop_cols.count(col)) continue;

            string val = trim(fields[i]);

            if (date_cols.count(col)) {
                bool ok = false;
                double doy = parse_date_to_day_of_year(val, ok);
                if (!ok) doy = 0.0;
                // guardamos como string numérica
                {
                    ostringstream oss;
                    oss << std::setprecision(17) << doy;
                    feat[col] = oss.str();
                }
                continue;
            }

            if (categorical_cols.count(col)) {
                feat[col] = val;
                cat_values[col].insert(val);
                continue;
            }

            // resto: float (si falla, forzamos error)
            double fv = 0.0;
            if (!try_double(val, fv)) {
                throw runtime_error(
                    "Valor no numerico en columna '" + col + "': '" + val +
                    "'. Si es categorica, incluyela en --cat_cols; si sobra, en --drop; si es fecha, en --date_cols."
                );
            }
            {
                ostringstream oss;
                oss << std::setprecision(17) << fv;
                feat[col] = oss.str();
            }
        }

        rows_feat_raw.push_back(feat);
    }

    // cat_map ordenado
    map<string, vector<string>> cat_map;
    for (auto& kv : cat_values) {
        vector<string> vals(kv.second.begin(), kv.second.end());
        sort(vals.begin(), vals.end());
        cat_map[kv.first] = vals;
    }

    if (rows_feat_raw.empty()) {
        throw runtime_error("No hay filas de datos en el CSV: " + path);
    }

    // numeric_cols: keys del primer row que NO son categóricas, luego sorted
    vector<string> numeric_cols;
    for (auto& kv : rows_feat_raw[0]) {
        const string& col = kv.first;
        if (!categorical_cols.count(col)) numeric_cols.push_back(col);
    }
    sort(numeric_cols.begin(), numeric_cols.end());

    // feature_names = numeric + one-hot
    vector<string> feature_names;
    for (auto& c : numeric_cols) feature_names.push_back(c);

    vector<string> cat_cols_sorted(categorical_cols.begin(), categorical_cols.end());
    sort(cat_cols_sorted.begin(), cat_cols_sorted.end());

    for (auto& c : cat_cols_sorted) {
        for (auto& v : cat_map[c]) {
            feature_names.push_back(c + "__" + v);
        }
    }

    // Construimos X
    vector<vector<double>> X;
    X.reserve(rows_feat_raw.size());

    for (auto& feat : rows_feat_raw) {
        vector<double> vec;
        vec.reserve(feature_names.size());

        // numéricas
        for (auto& col : numeric_cols) {
            auto it = feat.find(col);
            double val = 0.0;
            if (it != feat.end()) {
                if (!try_double(it->second, val)) val = 0.0; // por seguridad
            }
            vec.push_back(val);
        }

        // one-hot categóricas
        for (auto& c : cat_cols_sorted) {
            string current = "";
            auto it = feat.find(c);
            if (it != feat.end()) current = it->second;

            for (auto& v : cat_map[c]) {
                vec.push_back((current == v) ? 1.0 : 0.0);
            }
        }

        X.push_back(vec);
    }

    DatasetMixedResult out;
    out.X = std::move(X);
    out.y_str = std::move(y_str);
    out.feature_names = std::move(feature_names);
    out.cat_map = std::move(cat_map);
    return out;
}

// Normalización Min-Max [0..1]
static vector<vector<double>> minmax_normalize(const vector<vector<double>>& X) {
    int n = (int)X.size();
    int d = (int)X[0].size();

    vector<double> mins(d, std::numeric_limits<double>::infinity());
    vector<double> maxs(d, -std::numeric_limits<double>::infinity());

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < d; j++) {
            double v = X[i][j];
            if (v < mins[j]) mins[j] = v;
            if (v > maxs[j]) maxs[j] = v;
        }
    }

    vector<vector<double>> Xn(n, vector<double>(d, 0.0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < d; j++) {
            double denom = maxs[j] - mins[j];
            if (denom == 0.0) Xn[i][j] = 0.0;
            else Xn[i][j] = (X[i][j] - mins[j]) / denom;
        }
    }
    return Xn;
}

// Mezcla los indices 0...n-1 con una semilla y los separa en train/test según el porcentaje
static pair<vector<int>, vector<int>> split_idx(int n, double test_ratio, int seed) {
    vector<int> idx(n);
    for (int i = 0; i < n; i++) idx[i] = i;

    std::mt19937 rng(seed);
    std::shuffle(idx.begin(), idx.end(), rng);

    int cut = (int)((1.0 - test_ratio) * n);
    vector<int> tr(idx.begin(), idx.begin() + cut);
    vector<int> te(idx.begin() + cut, idx.end());
    return {tr, te};
}

//Devuelve una copia del vector v cogiendo solo las posiciones indicadas por idx (para sacar y_train, y_test)
template <typename T>
static vector<T> subset_vec(const vector<T>& v, const vector<int>& idx) {
    vector<T> out;
    out.reserve(idx.size());
    for (int i : idx) out.push_back(v[i]);
    return out;
}

//Igual que subset_vec pero para matrices X. Devuelve solo las filas indicadas
static vector<vector<double>> subset_X(const vector<vector<double>>& X, const vector<int>& idx) {
    vector<vector<double>> out;
    out.reserve(idx.size());
    for (int i : idx) out.push_back(X[i]);
    return out;
}


// Perceptrón binario + OvR
static vector<double> train_perceptron(const vector<vector<double>>& X, const vector<int>& y01,
                                       int epochs, double lr, int seed) {
    int d = (int)X[0].size();
    vector<double> w(d + 1, 0.0);

    std::mt19937 rng(seed);

    for (int ep = 0; ep < epochs; ep++) {
        vector<int> order((int)X.size());
        for (int i = 0; i < (int)X.size(); i++) order[i] = i;
        std::shuffle(order.begin(), order.end(), rng);

        for (int ii : order) {
            const auto& xi = X[ii];
            int yi = y01[ii];

            double s = w[d]; // bias
            for (int j = 0; j < d; j++) s += w[j] * xi[j];

            int pred = (s >= 0.0) ? 1 : 0;
            int err = yi - pred;

            if (err != 0) {
                for (int j = 0; j < d; j++) w[j] += lr * err * xi[j];
                w[d] += lr * err;
            }
        }
    }
    return w;
}

//Calcula la puntución lineal (s=w*x+b) sin aplicar umbral. Se usa para ver qué clase gana en OvR
static double score_binary(const vector<double>& x, const vector<double>& w) {
    int d = (int)x.size();
    double s = w[d];
    for (int j = 0; j < d; j++) s += w[j] * x[j];
    return s;
}

//Entrena un perceptrón binario por cada clase y devuelve todos los modelos guardados en un mapa
static map<int, vector<double>> train_ovr(const vector<vector<double>>& X, const vector<int>& y,
                                         const vector<int>& classes, int epochs, double lr, int seed) {
    map<int, vector<double>> W;
    for (int k : classes) {
        vector<int> y01;
        y01.reserve(y.size());
        for (int yi : y) y01.push_back((yi == k) ? 1 : 0);

        W[k] = train_perceptron(X, y01, epochs, lr, seed + 10 * k);
    }
    return W;
}

//Predice la clase calculando el score de cada perceptrón y devolviendo la clase con puntuación más alta
static int predict_ovr(const vector<double>& x, const map<int, vector<double>>& W, const vector<int>& classes) {
    int best_k = -1;
    double best_s = 0.0;
    bool first = true;

    for (int k : classes) {
        double s = score_binary(x, W.at(k));
        if (first || s > best_s) {
            best_s = s;
            best_k = k;
            first = false;
        }
    }
    return best_k;
}


// Construye la matriz de confusión multiclase contando aciertos/errores por clase
static vector<vector<int>> confusion_matrix_multiclass(const vector<int>& y_true,
                                                       const vector<int>& y_pred,
                                                       int K) {
    vector<vector<int>> cm(K, vector<int>(K, 0));
    for (size_t i = 0; i < y_true.size(); i++) {
        int t = y_true[i];
        int p = y_pred[i];
        cm[p][t] += 1; // filas=Pred, columnas=Real
    }
    return cm;
}

// Calcula el porcentaje de predicciones correctas
static double accuracy(const vector<int>& y_true, const vector<int>& y_pred) {
    int ok = 0;
    for (size_t i = 0; i < y_true.size(); i++) if (y_true[i] == y_pred[i]) ok++;
    return y_true.empty() ? 0.0 : (double)ok / (double)y_true.size();
}

//A partir de la matriz de confusión calcula precision,recall y F1 de cada clase y luego hace el promedio macro
static void precision_recall_f1_macro(const vector<vector<int>>& cm,
                                      double& p_macro, double& r_macro, double& f1_macro) {
    int K = (int)cm.size();
    vector<double> ps, rs, f1s;

    for (int k = 0; k < K; k++) {
        int tp = cm[k][k];
        int fp = 0, fn = 0;
        for (int j = 0; j < K; j++) if (j != k) fp += cm[k][j];
        for (int i = 0; i < K; i++) if (i != k) fn += cm[i][k];

        double prec = (tp + fp) ? (double)tp / (double)(tp + fp) : 0.0;
        double rec  = (tp + fn) ? (double)tp / (double)(tp + fn) : 0.0;
        double f1   = (prec + rec) ? (2.0 * prec * rec / (prec + rec)) : 0.0;

        ps.push_back(prec);
        rs.push_back(rec);
        f1s.push_back(f1);
    }

    auto mean = [](const vector<double>& v) {
        double s = 0.0;
        for (double x : v) s += x;
        return v.empty() ? 0.0 : s / (double)v.size();
    };

    p_macro = mean(ps);
    r_macro = mean(rs);
    f1_macro = mean(f1s);
}

// Imprime la matriz de confusión 
static void print_confusion_matrix_multiclass(const vector<vector<int>>& cm,
                                              const vector<string>& class_names) {
    cout << "\nMatriz de confusión (Pred vs Real):\n";
    cout << string(16, ' ');
    for (auto& name : class_names) {
        cout << "  " << "Real=" << setw(8) << name;
    }
    cout << "\n";
    for (int i = 0; i < (int)cm.size(); i++) {
        cout << "Pred=" << setw(8) << class_names[i] << "  ";
        for (int v : cm[i]) cout << setw(10) << v << "  ";
        cout << "\n";
    }
}


// Parseo de argumentos 
struct Args {
    string csv;
    string target;
    string classes;
    string drop;
    string date_cols;
    string cat_cols;
    int epochs = 20;
    double lr = 0.1;
    double test_ratio = 0.2;
    int seed = 0;
};

//Lee los argumentos de consola, los valida, y devuelve una estructura Args con todo
static Args parse_args(int argc, char** argv) {
    Args a;
    for (int i = 1; i < argc; i++) {
        string k = argv[i];
        auto need = [&](const string& opt) {
            if (i + 1 >= argc) {
                cerr << "Falta valor para " << opt << "\n";
                usage(argv[0]);
            }
            return string(argv[++i]);
        };

        if (k == "--csv") a.csv = need(k);
        else if (k == "--target") a.target = need(k);
        else if (k == "--classes") a.classes = need(k);
        else if (k == "--drop") a.drop = need(k);
        else if (k == "--date_cols") a.date_cols = need(k);
        else if (k == "--cat_cols") a.cat_cols = need(k);
        else if (k == "--epochs") a.epochs = stoi(need(k));
        else if (k == "--lr") a.lr = stod(need(k));
        else if (k == "--test_ratio") a.test_ratio = stod(need(k));
        else if (k == "--seed") a.seed = stoi(need(k));
        else {
            cerr << "Opcion desconocida: " << k << "\n";
            usage(argv[0]);
        }
    }

    if (a.csv.empty() || a.target.empty() || a.classes.empty()) {
        cerr << "Faltan argumentos obligatorios (--csv, --target, --classes)\n";
        usage(argv[0]);
    }
    return a;
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    try {
        Args args = parse_args(argc, argv);

        vector<string> classes_str = split_by_comma(args.classes);
        if (classes_str.size() < 2) {
            throw runtime_error("Debes pasar al menos 2 clases en --classes");
        }

        vector<string> drop_cols = split_by_comma(args.drop);
        vector<string> date_cols = split_by_comma(args.date_cols);
        vector<string> cat_cols  = split_by_comma(args.cat_cols);

        //  Cargar y vectorizar dataset
        DatasetMixedResult ds = load_dataset_mixed_types(
            args.csv,
            args.target,
            drop_cols,
            date_cols,
            cat_cols
        );

        //  Mapear clases string -> int (0..K-1)
        map<string, int> class_to_int;
        map<int, string> int_to_class;
        for (int i = 0; i < (int)classes_str.size(); i++) {
            class_to_int[classes_str[i]] = i;
            int_to_class[i] = classes_str[i];
        }

        vector<int> y;
        y.reserve(ds.y_str.size());
        for (auto& s : ds.y_str) {
            // si no está, lanzamos error
            if (!class_to_int.count(s)) {
                throw runtime_error("Etiqueta '" + s + "' no aparece en --classes. Revisa el CSV o el argumento.");
            }
            y.push_back(class_to_int[s]);
        }

        int K = (int)classes_str.size();
        int n = (int)ds.X.size();
        int d = (int)ds.X[0].size();

        // Balance de clases
        vector<int> counts(K, 0);
        for (int yi : y) counts[yi]++;

        cout << "Dataset: " << args.csv << "\n";
        cout << "Samples: " << n << "\n";
        cout << "Features: " << d << "\n";
        cout << "Target: " << args.target << " -> [";
        for (int i = 0; i < K; i++) {
            cout << "'" << classes_str[i] << "'";
            if (i + 1 < K) cout << ", ";
        }
        cout << "]\n";
        cout << "Class balance:\n";
        for (int i = 0; i < K; i++) {
            cout << "  " << classes_str[i] << " -> " << counts[i] << "\n";
        }

        //  Normalizar
        vector<vector<double>> Xn = minmax_normalize(ds.X);

        //  Split
        auto spl = split_idx(n, args.test_ratio, args.seed);
        vector<int> tr = spl.first;
        vector<int> te = spl.second;

        vector<vector<double>> Xtr = subset_X(Xn, tr);
        vector<int> ytr = subset_vec(y, tr);
        vector<vector<double>> Xte = subset_X(Xn, te);
        vector<int> yte = subset_vec(y, te);

        cout << "Split: train=" << tr.size() << ", test=" << te.size() << "\n";

        // Entrenar OvR
        vector<int> classes_int;
        for (int i = 0; i < K; i++) classes_int.push_back(i);

        map<int, vector<double>> W = train_ovr(
            Xtr, ytr, classes_int,
            args.epochs, args.lr,
            args.seed + 1
        );

        // Evaluar
        vector<int> ypred;
        ypred.reserve(Xte.size());
        for (auto& x : Xte) ypred.push_back(predict_ovr(x, W, classes_int));

        double acc = accuracy(yte, ypred);
        vector<vector<int>> cm = confusion_matrix_multiclass(yte, ypred, K);

        double p_macro = 0.0, r_macro = 0.0, f1_macro = 0.0;
        precision_recall_f1_macro(cm, p_macro, r_macro, f1_macro);

        cout << "\nResultados en TEST :\n";
        cout << "epochs=" << args.epochs << ", lr=" << args.lr << "\n";
        cout << fixed << setprecision(4);
        cout << "Accuracy     : " << acc << "\n";
        cout << "Precision(M) : " << p_macro << "\n";
        cout << "Recall(M)    : " << r_macro << "\n";
        cout << "F1(M)        : " << f1_macro << "\n";

        vector<string> class_names;
        for (int i = 0; i < K; i++) class_names.push_back(int_to_class[i]);
        print_confusion_matrix_multiclass(cm, class_names);

    } catch (const exception& e) {
        cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
