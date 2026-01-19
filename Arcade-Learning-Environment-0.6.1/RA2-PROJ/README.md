**INSTRUCCIONES DE EJECUCIÓN** <br>

**PERCEPTRÓN** <br>
Desde la carpeta donde está perceptrón_runner.cpp : <br>
    g++ -std=c++11 -O2 -o perceptron_runner perceptron_runner.cpp

Esto generará el ejecutable perceptron_runner. Para ejecutarlo debemos escribir lo siguiente por línea de comandos : <br>
    ./perceptron_runner --csv <ruta_csv> --target <columna_objetivo> --classes "<c1,c2,...>" [opciones...]

Las opciones disponibles son (Todas opcionales) : 
   - --drop "<col1,col2,...>" : columnas a eliminar
   - --date_cols "<col1,col2,...>" : columnas de fecha (se convierten a día del año)
   - --cat_cols "<col1,col2,...>" : columnas categóricas (one-hot encoding)
   - --epochs N (por defecto 20)
   - --lr X ( por defecto 0.1)
   - --test_ratio R ( por defecto 0.2)
   - --seed S ( por defecto 0)

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



**RED NEURONAL CON PESOS GENÉTICOS** <br>
Para probar la red neuronal ir a **pruebaERRNN.cpp** <br>
Antes de ejecutar cualquier prueba hay tres parámetros en el codigo que se pueden cambiar:
- INPUTS_SIZE: Número de filas con las que entrenar la red, se evaluará con el resto
- TOURNAMENT_ROUNDS: Número de rondas que se harán para obtener la mejor red (epochs).
- POPULATION_SIZE: Número de redes con las que se hace el torneo.

También dentro encontrarás funciones para cada uno de los tipos de usos de la red:
- Clasificación
    - fitness_class: Suma valores positivos proporcionales a cuanto de cerca está del lado al que deberían estar y son penalizados por estar en el contrario
    - test_class: Aporta información como el recall de cada una de las clases, la matriz de difusión y el porcentaje de acierto total

- Predicción
    - fitness_pred: Suma valores positivos proporcionales dependiendo dentro de que umbral cae o negativos si cae fuera de cualquiera
    - test_pred: Imprime unicamente el error medio 

Luego en la línea 250, está disponible para modificar la arquitectura de la red. Actualmente en el fichero esta para probar mental_health_social_media_dataset.csv. Si se quisiera probar la red con la funcion de clasificación habría que cambiar en el main la llama de test_pred por test_class y en la función train, donde pone fitness_pred sustituirlo por fitness_class, y por útlimo utilizar el parser correspondiente a cualquiera de las opciones que se quiera probar, como lo son Heart_Disease_Prediction e Iris_Shuffled. 
