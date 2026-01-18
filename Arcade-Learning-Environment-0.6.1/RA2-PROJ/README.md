**INSTRUCCIONES DE EJECUCIÓN**

**PERCEPTRÓN**
Desde la carpeta donde está perceptrón_runner.cpp : 
    g++ -std=c++11 -O2 -o perceptron_runner perceptron_runner.cpp

Esto generará el ejecutable perceptron_runner. Para ejecutarlo debemos escribir lo siguiente por línea de comandos : 
    ./perceptron_runner --csv <ruta_csv> --target <columna_objetivo> --classes "<c1,c2,...>" [opciones...]

Las opciones disponibles son (Todas opcionales) : 
    --drop "<col1,col2,...>" : columnas a eliminar
    --date_cols "<col1,col2,...>" : columnas de fecha (se convierten a día del año)
    --cat_cols "<col1,col2,...>" : columnas categóricas (one-hot encoding)
    --epochs N (por defecto 20)
    --lr X ( por defecto 0.1)
    --test_ratio R ( por defecto 0.2)
    --seed S ( por defecto 0)

**RED NEURONAL CON BACKPROPAGATION**

- Prueba inicial de red con backpropagation: 
    - g++ -o pbp entrenamiento_bp_prueba.cpp
    - ./pbp

- Heart disease predictor con backpropagation: 
    - g++ -o hdbp heartdisease_bp.cpp
    - ./hdbp

- Predictor horas de sueño con mental health social media: 
    - g++ -o sbp sleep_bp.cpp
    - ./sbp

- Predictor categoría de Iris:
    - g++ -o ibp iris_bp.cpp
    - ./ibp

al ejecutarlo pedirá introducir una función de activación (s: sigmoid, r:relu, t:tanh)



**RED NEURONAL CON PESOS GENÉTICOS**