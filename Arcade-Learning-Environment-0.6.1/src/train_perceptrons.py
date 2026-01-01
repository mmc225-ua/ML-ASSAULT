import csv
import random

"""
- Leemos todas las filas del CSV (all_runs)
- Convertimos cada fila en un ejemplo de aprendizaje:
    + X = 128 entradas (RAM normalizada)
    + yL = 1 si en ese frame se pulsó LEFT (o LEFTFIRE), si no 0
    + yR = 1 si en ese frame se pulsó RIGHT (o RIGHTFIRE), si no 0
    + yF = 1 si en ese frame se disparó (UPFIRE / LEFTFIRE / RIGHTFIRE), si no 0
- Dividimos los ejemplos en train y test (80 / 20 ) 
- Entrenamos 3 perceptrones (uno para yL, otro para yR, otro para yF)
- Evaluamos cada perceptrón con accuracy/precisión/recall
- Escribimos los pesos en un fichero cabecera C++ "weights_assault.hpp" 
"""
# Conjunto de datos para entrenar los perceptrones
DATASET = "../../all_runs.csv"

# Valores ALE asociados a las acciones de juego
LEFT_ACTIONS  = {3, 11} # Left, LeftFire
RIGHT_ACTIONS = {4, 12} # Right, RightFire
FIRE_ACTIONS  = {10, 11, 12} #Upfire, LeftFire, RightFire

def load_dataset(path):
    X, yL, yR, yF = [], [], [], [] #Rellena las cuatro listas
    with open(path) as f:
        r = csv.DictReader(f)
        for row in r:
            #Crea una lista de 128 elementos, y para cada uno, lee : row["ram0"], row["ram1"], … row["ram127"]
            #Luego convierte cada uno a entero con int y los normaliza dividiendolos entre 255.
            x = [int(row[f"ram{i}"]) / 255.0 for i in range(128)]

            # a = acción usada en ese frame (Left(3), Upfire(10)...)
            a = int(row["action"])

            X.append(x)
            yL.append(1 if a in LEFT_ACTIONS else 0)
            yR.append(1 if a in RIGHT_ACTIONS else 0)
            yF.append(1 if a in FIRE_ACTIONS else 0)
    return X, yL, yR, yF

#Separa los ejemplos en conjunto de entrenamiento y conjunto de test, de forma aleatoria
# n -> número total de ejemplos
# test_ratio -> porcentaje que queremos reservar para test (0.2 significa 20% test, 80% train)
# seed -> semilla del generador aleatorio
def split_idx(n, test_ratio=0.2, seed=0):
    idx = list(range(n)) # Creamos los índices en una lista
    random.seed(seed) # Asignamos la semilla
    random.shuffle(idx)# Mezclamos los índices 
    cut = int((1 - test_ratio) * n) # Si n=100 y test_ratio = 0.2 , Cut = 80, indicando que los primeros 80 serán de entrenamiento
    return idx[:cut], idx[cut:] #Devuelve como primer parámetro una lista con los índices de entrenamiento, y como segundo una lista con los índices de test

def subset(X, y, idx): #De todo el conjunto X con sus respectivas y , devuelve solo dos listas X y con los índices incluidos en idx
    return [X[i] for i in idx], [y[i] for i in idx]

def train_perceptron(X, y, epochs=6, lr=0.1, seed=0):
    random.seed(seed)# Fijamos la semilla
    d = len(X[0]) # Dimensión de entrada
    w = [0.0] * (d + 1)  # Creamos el vector de pesos donde el último es el bias
    for _ in range(epochs): # Para cada época ...
        order = list(range(len(X)))
        random.shuffle(order) # Mezclamos los ejemplos en cada época, para que no influya el órden 
        for i in order:
            xi, yi = X[i], y[i] 
            s = w[d] #bias
            for j in range(d):
                # w[j] -> peso de esta neurona (importancia de ese valor para el aprendizaje)
                # xi[j] -> valor normalizado de la ram
                s += w[j] * xi[j] 
            pred = 1 if s >= 0 else 0 # s negativo -> NO pulsar /// s positivo -> SI pulsar
            err = yi - pred # Comprobamos si la predicción (+1 / -1 ) es igual a la acción tomada por el jugador
            if err != 0: # Si no es igual, ajustamos los pesos de cada neurona
                for j in range(d): # Para cada peso, se le acerca [LearningRate] unidades en dirección positiva o negativa, 
                                # en función de si la acción ha sido pulsar (positiva) o no pulsar (negativa)
                    w[j] += lr * err * xi[j]
                w[d] += lr * err # EL bias también se actualiza según el LearningRate
    return w # Devuelve el vector de pesos tamaño 129 (w[0..127]: pesos de la RAM / w[128]: bias)

def predict(x, w): # Usamos los pesos aprendidos para predecir si pulsar o no 
    d = len(x)
    s = w[d]
    for j in range(d):
        s += w[j] * x[j]
    return 1 if s >= 0 else 0

# X -> Ejemplos de ram normalizada
# y -> etiquetas reales (0/1)
# w -> pesos aprendidos
def metrics(X, y, w):# Comprueba las estadísticas de acierto/fallo de nuestro vector de pesos
    tp = fp = tn = fn = 0
    for xi, yi in zip(X, y): # Con zip se recorren 2 listas a la vez, así se van rellenando "xi" y "yi" a la vez según los índices de "X" e "y"
        p = predict(xi, w)
        if yi == 1 and p == 1: tp += 1
        elif yi == 0 and p == 1: fp += 1
        elif yi == 0 and p == 0: tn += 1
        else: fn += 1
    acc = (tp + tn) / (tp + tn + fp + fn)
    prec = tp / (tp + fp) if (tp + fp) else 0.0
    rec = tp / (tp + fn) if (tp + fn) else 0.0
    return acc, prec, rec, (tp, fp, tn, fn) # Devuelve accuracy, precision, recall y Número de : (TruePositive, FalsePositive, TrueNegative, FalseNegative)

def dump_cpp(name, w): # Construye el vector de pesos en un vector de float con 6 decimales para que nos sirva en C++
    vals = ", ".join(f"{v:.6f}f" for v in w)
    return f"static const float {name}[129] = {{ {vals} }};\n"

def main():
    X, yL, yR, yF = load_dataset(DATASET) # Cargamos el dataset
    n = len(X)#Número de ejemplos
    tr, te = split_idx(n, 0.2, seed=0) # Partición train test

    # Construimos subjonjuntos de entrenamiento
    Xtr, yLtr = subset(X, yL, tr)
    _,   yRtr = subset(X, yR, tr)
    _,   yFtr = subset(X, yF, tr)

    #Construimos subconjuntos de test
    Xte, yLte = subset(X, yL, te)
    _,   yRte = subset(X, yR, te)
    _,   yFte = subset(X, yF, te)

    print(f"Samples: {n} (train {len(tr)}, test {len(te)})")

    #Entrenamos los 3 perceptrones
    wL = train_perceptron(Xtr, yLtr, epochs=10, lr=0.1, seed=1)
    wR = train_perceptron(Xtr, yRtr, epochs=10, lr=0.1, seed=2)
    wF = train_perceptron(Xtr, yFtr, epochs=10, lr=0.1, seed=3)

    #Mostramos las métricas para cada uno
    print("LEFT : acc,prec,rec,cm =", metrics(Xte, yLte, wL))
    print("RIGHT: acc,prec,rec,cm =", metrics(Xte, yRte, wR))
    print("FIRE : acc,prec,rec,cm =", metrics(Xte, yFte, wF))

    #Guardamos los pesos en un fichero .hpp para luego incluirlo en nuestro .cpp
    with open("weights_assault.hpp", "w") as f:
        f.write("// Pesos generados con train_perceptrons.py\n\n")
        f.write(dump_cpp("wLEFT", wL))
        f.write(dump_cpp("wRIGHT", wR))
        f.write(dump_cpp("wFIRE", wF))

    print("Saved: weights_assault.hpp")

if __name__ == "__main__":
    main()
