#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#define N 2^(10)

//int M = 124;
//int n = 2^20; --> 1.048.576
// S = (n/m)^1/2 --> S*S*M = 91 * 91 * 124 = 1.026.844
// Osea se van a perder rectangulos por el metodo de construcción, que serán creo los mas alejados
int buildRTreebySTR(char *dirArchivoEntrada, char *dirArchivoSalida, int M, int N);
void insertarOrdenadoX(int* Objetivo, int ptr_obj, int* Pozo, int S, int M, int N);
void insertarOrdenadoY(int* Objetivo, int ptr_obj, int* Pozo,int inicioPozo, int finalPozo, int N);
void guardarHoja(int* Objetivo, int inicio, int M, FILE* archivoGuardado, long* pos_Actual);
void CrearNodos(FILE* archivoGuardado, long* pos_Busqueda, long* pos_Actual, int* N_cambiable,int M, int N);

int buildRTreebySTR(char *dirArchivoEntrada, char *dirArchivoSalida, int M, int N) {

    FILE *archivoEntrada, *archivoIntermedio;
    char *nombreArchivoIntermedio = "intermedio.bin";

    // Abrir el archivo de entrada en modo lectura binaria
    archivoEntrada = fopen(dirArchivoEntrada, "rb");
    if (archivoEntrada == NULL) {
        perror("Error al abrir el archivo de entrada");
        return 1;
    }

    // Abrir el archivo de salida en modo escritura binaria
    archivoIntermedio = fopen(nombreArchivoIntermedio, "wb");
    if (archivoIntermedio == NULL) {
        perror("Error al abrir el archivo de salida");
        fclose(archivoEntrada);
        return 1;
    }

    int buffer[4];  // Buffer para almacenar los enteros
    int indice = -1;
    // Leer y escribir enteros en grupos de TAMANO_GRUPO
    while (fread(buffer, sizeof(int), 4, archivoEntrada) == 4) {
        fwrite(buffer, sizeof(int), 4, archivoIntermedio);
        fwrite(&indice,sizeof(int),1,archivoIntermedio);
        indice--;
    }

    // Cerrar los archivos
    fclose(archivoEntrada);
    fclose(archivoIntermedio);


/*

    //crear archivo para almacenar arbol del metodo 3
    //Leer archivo y guardar los puntos en un array
    FILE *archivo;
    int* Z = (int*) malloc(N * sizeof(int)*4) ;  // Array para almacenar los enteros
    int num_elementos = 0; 
    int num_ptr = 1;

    archivo = fopen(dirArchivoEntrada, "rb");

    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }
    while (fread(&Z[num_elementos], sizeof(int), 4, archivo) == 4) {
        num_elementos += 4;
        Z[num_elementos] = -num_ptr; 
        num_ptr++;
        num_elementos ++;
        if (num_elementos >= N*5) {
            break;
        }
    }
    fclose(archivo);

    printf("Integers from the array: ");
    for (int i = 0; i < N*sizeof(int)*4; i++) {
        printf("%d ", Z[i]);
    }
*/

    // Open the binary file for reading
    FILE *archivo = fopen(nombreArchivoIntermedio, "rb");

    // Check if the file was opened successfully
    if (archivo == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    // Get the size of the file
    fseek(archivo, 0, SEEK_END);
    long file_size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);

    // Calculate the number of integers in the file
    size_t num_integers = file_size / sizeof(int);

    // Allocate memory for the array using malloc
    int *Z = (int *)malloc(num_integers * sizeof(int));

    // Check if malloc was successful
    if (Z == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        fclose(archivo);
        return 1;
    }

    // Read integers from the file into the array
    fread(Z, sizeof(int), num_integers, archivo);

    // Close the file
    fclose(archivo);

    printf("number of ints: %ld\n", num_integers);
    printf("number of rects: %d\n", N);

/*
    printf("Integers from the array: \n");
    for (int i = 0; i < N; ++i) {
        // Extracting coordinates for the current rectangle
        int x1 = Z[i * 4];
        int y1 = Z[i * 4 + 1];
        int x2 = Z[i * 4 + 2];
        int y2 = Z[i * 4 + 3];

        // Calculating center coordinates
        int centerX = (x1 + x2) / 2;
        int centerY = (y1 + y2) / 2;

        // Printing the information
        printf("Rectangle %d: (%d, %d, %d, %d) - Center: (%d, %d)\n", i + 1, x1, y1, x2, y2, centerX, centerY);
    }
*/

    //Cte S
    int S = pow((N/M),0.5); // aprox 92
    printf("valor de S: %d\n", S);
    
    //int Y[92][92*124] = malloc(N*sizeof(int)*4);

    //Creamos un Array nuevo para almacenar los elementos ahora Ordenados por la coordenada X en grupos de S*M
    int* Y = (int*) malloc(N * sizeof(int)*5) ;

    printf("Integers from the array YYYYY: ");
    for (int i = 0; i < N*sizeof(int)*5; i++) {
        printf("%d ", Y[i]);
    }

    //Metemos para cada grupo de S*M elementos los rectangulos ordenados por su coordenada X
    //for(int v = 0; v < S; v++){
    for(int v = 0; v < N; v++){     // Dato curioso, aqui vamos a recorrer todo el array pero quedó con doble for por una estructura pasada XD
        //for(int t = 0; t < S*M;t++){
            insertarOrdenadoX(Y,v*5,Z,S,M,N); // Aqui no estoy seguro si a la dirección se le suman esos numeros por el sizeof(int) o como está
        //}
    } 

    printf("VALOR DE SSM=%d\n", S*S*M);

    printf("--------------\n");
    printf("Integers from the ordered by X array: \n");
    for (int i = 0; i < N; ++i) {
        // Extracting coordinates for the current rectangle
        int x1 = Y[i * 5];
        int y1 = Y[i * 5 + 1];
        int x2 = Y[i * 5 + 2];
        int y2 = Y[i * 5 + 3];
        int ind = Y[i * 5 + 4];

        // Calculating center coordinates
        int centerX = (x1 + x2) / 2;
        int centerY = (y1 + y2) / 2;

        // Printing the information
        printf("Rectangle %d: (%d, %d, %d, %d) - Center: (%d, %d)\n", i + 1, x1, y1, x2, y2, centerX, centerY);
    }

    printf("CHECKPOINT: \n");

    // Array para Ordenar por Y--> actualmente están ordenados por X y se busca separarlos en grupos de S*M antes de ordenarlos por Y
    int* X = (int*) malloc(N * sizeof(int)*5) ;
    printf("CHECKPOINT2222: \n");
    // Le insertamos los elementos ordenados por Y
    for(int a = 0; a < S; a++){ 
        printf("grupo: %d\n", a);
        for(int b = 0; b < S*M; b++){
            printf("valor de b: %d\n", b);
            printf("valor de a*S*M+b: %d\n", a*S*M+b);
            insertarOrdenadoY( X, ((a*S*M)+ b)*5, Y, a*S*M, (a*S*M) + (S*M), N);
        }
    }


    //a=grupo
    //b=lugar donde se escribe x

    for(int c = 0; c < N-S*S*M;c++){
        insertarOrdenadoY(X,((S*S*M)+c)*5,Y,S*S*M,N,N);
        //insertarOrdenadoY(X,((S*S*M)+c)*5,Y,S*S*M,(S*S*M)+N,N);
    }

    printf("Integers from the ordered by Y array: \n");
    for (int i = 0; i < N; ++i) {
        // Extracting coordinates for the current rectangle
        int x1 = X[i * 5];
        int y1 = X[i * 5 + 1];
        int x2 = X[i * 5 + 2];
        int y2 = X[i * 5 + 3];
        int ind = X[i * 5 + 4];

        // Calculating center coordinates
        int centerX = (x1 + x2) / 2;
        int centerY = (y1 + y2) / 2;

        // Printing the information
        printf("Rectangle %d: (%d, %d, %d, %d) - Center: (%d, %d)\n", i + 1, x1, y1, x2, y2, centerX, centerY);
    }

    // Ahora estos son los nodos hoja, toca guardarlos en un archivo
    //Ahora tenemos S*S hojas con M rectangulos c/u
    FILE* archivoGuardado;
    archivoGuardado = fopen(dirArchivoSalida, "wb");

    if (archivoGuardado == NULL) {
        perror("Error al abrir el archivo de guardado");
        return 1;
    }

    long* pos_Actual;
    long* pos_Busqueda;
    pos_Busqueda = (long*)malloc(sizeof(long));
    pos_Actual = (long*)malloc(sizeof(long));
    *pos_Busqueda = sizeof(int)*(4+M);

    
    fseek(archivoGuardado, sizeof(int)*(4+M), SEEK_SET);
    for(int a = 0; a < S*S; a++){
        printf("algo xd\n");
        guardarHoja(X, a, M, archivoGuardado,pos_Actual); // Esta función debería guardar la hoja con los punteros a los M rectangulos
    }

    int NumNodosActuales = S*S;
    free(X);
    free(Y);
    free(Z);
    while(NumNodosActuales > 1){
        CrearNodos(archivoGuardado,pos_Busqueda,pos_Actual,&NumNodosActuales,M, N); // idea: el arbol se hace de abajo hacia arriba, guardamos los nodos desde la 2da posición del archivo para dejar a la Raiz primero
    }

    return 0;
}

void insertarOrdenadoX(int* Objetivo, int ptr_obj, int* Pozo, int S, int M, int N) {
    //Esta función agrega a Objetivo el nodo de menor valor medio de la coordenada X, del Array Pozo y lo elimina de este (ahora vale -1 y no va a contar para los siguientes usos)
    int pos = 999999999; // posición del elegido
    int xd = 999999999;

    for(int a = 0; a < N*5 ;a += 5){ // buscamos el elemento de menor centro en X para agregar a Objetivo
        if(Pozo[a] > -1){
            printf("RECTANGULO: %d, %d, %d, %d\n", Pozo[a], Pozo[a+1], Pozo[a+2], Pozo[a+3]);
            if((Pozo[a] + Pozo[a+2])/2 < xd){
                pos = a; 
                xd=(Pozo[a] + Pozo[a+2])/2;
            }
        }
    }
    Objetivo[ptr_obj  ] = Pozo[pos];
    Objetivo[ptr_obj+1] = Pozo[pos+1];
    Objetivo[ptr_obj+2] = Pozo[pos+2];
    Objetivo[ptr_obj+3] = Pozo[pos+3];
    Objetivo[ptr_obj+4] = Pozo[pos+4];
    Pozo[pos  ] = -1;
    Pozo[pos+1] = -1;
    Pozo[pos+2] = -1;
    Pozo[pos+3] = -1;
    Pozo[pos+4] = -1;
}

void insertarOrdenadoY(int* Objetivo, int ptr_obj, int* Pozo,int inicioPozo, int finalPozo, int N) {
    //Esta función agrega a Objetivo el nodo de menor valor medio de la coordenada Y, del Array Pozo y lo elimina de este (ahora vale -1 y no va a contar para los siguientes usos)
    int pos = 999999999; // posición del elegido
    int xd = 999999999;

    for(int a = inicioPozo; a < finalPozo;a ++){ // buscamos el elemento de menor centro en Y para agregar a Objetivo
        //printf("CHECKPOINT valor de aaaaaaa: %d\n", a);
        if(Pozo[a*5] > -1){
            if((Pozo[a*5+1] + Pozo[a*5+3])/2 < xd){
                pos = a*5; 
                xd=(Pozo[a*5+1] + Pozo[a*5+3])/2;
            }
        }
    }
    //printf("CHECKPOINT inicio y final: %d, %d\n", inicioPozo, finalPozo);
    //printf("VALOR DE POZO : %d, %d, %d, %d, %d. \n", Pozo[pos], Pozo[pos+1], Pozo[pos+2], Pozo[pos+3], Pozo[pos+4]);
    //printf("CHECKPOINT objetivo Y con ptr_obj: %d\n", ptr_obj);
    //printf("CHECKPOINT pozo Y con pos: %d\n", pos);
    Objetivo[ptr_obj  ] = Pozo[pos];
    Objetivo[ptr_obj+1] = Pozo[pos+1];
    Objetivo[ptr_obj+2] = Pozo[pos+2];
    Objetivo[ptr_obj+3] = Pozo[pos+3];
    Objetivo[ptr_obj+4] = Pozo[pos+4];
    //printf("CHECKPOINT con valor de ptr_obj: %d\n", ptr_obj);
    Pozo[pos  ] = -1;
    Pozo[pos+1] = -1;
    Pozo[pos+2] = -1;
    Pozo[pos+3] = -1;
    Pozo[pos+4] = -1;
}

void guardarHoja(int* Objetivo, int inicio, int M, FILE* archivoGuardado, long* pos_Actual) {
    // esta función guarda todas las hojas de Objetivo con rectangulos desde inicio hasta inico + M en archivoGuardado
    // Primero buscamos los 4 puntos de la hoja
    int Xmayor = 0;
    int Xmenor = 0;
    int Ymayor = 0; 
    int Ymenor = 0;
    for(int ptr = 0; ptr < M; ptr++){ // buscamos X e Y mayor y menor
        if(Objetivo[ inicio*(M+5)+(ptr*5)    ] < Xmenor){
            Xmenor = Objetivo[inicio*(M+5)+(ptr*5)];
        }
        if(Objetivo[ inicio*(M+5)+(ptr*5) + 1] < Ymenor){
            Ymenor = Objetivo[inicio*(M+5)+(ptr*5) + 1];
        }
        if(Objetivo[ inicio*(M+5)+(ptr*5) + 2] > Xmayor){
            Xmayor = Objetivo[inicio*(M+5)+(ptr*5) + 2];
        }
        if(Objetivo[ inicio*(M+5)+(ptr*5) + 3] > Ymayor){
            Ymayor = Objetivo[inicio*(M+5)+(ptr*5) + 3];
        }
    }
    // Teniendo los representates de la hoja toca escribir en el archivo los 4 puntos representantes y los punteros a los rectangulos
    int* hoja = (int*) malloc(sizeof(int)*(M+4)) ;
    int rectangulo = 4;
    hoja[0] = Xmenor;
    hoja[1] = Ymenor;
    hoja[2] = Xmayor;
    hoja[3] = Ymayor;
    while(rectangulo < M+4){
        hoja[rectangulo] = Objetivo[inicio*(rectangulo-4)];
        rectangulo++;
    }
    fwrite(hoja, sizeof(int), 4+M, archivoGuardado);
    *pos_Actual = ftell(archivoGuardado);
    free(hoja);
}

void CrearNodos(FILE* archivoGuardado, long* pos_Busqueda, long* pos_Actual, int* N_cambiable,int M, int N) { // el S_cambiable es el nuevo N 
    // Creamos array de los nodos anteriores
    int* Z = (int*) malloc(*N_cambiable * sizeof(int)*5) ;  // Array para almacenar los enteros
    int num_elementos = 0; 

    while (num_elementos < *N_cambiable*5 && *pos_Busqueda < *pos_Actual) { // Conseguimos que Z sea un array con los rectangulos del archivo
        fseek(archivoGuardado,*pos_Busqueda,SEEK_SET);
        fread(&Z[num_elementos], sizeof(int), 4, archivoGuardado);
        num_elementos += 4;
        Z[num_elementos] = (int) *pos_Busqueda; 
        *pos_Busqueda = *pos_Busqueda + (M+4*sizeof(int));
        num_elementos ++;
    }
    // Ahora basicamente hacemos lo mismo que antes
    int S = pow(*N_cambiable/M,0.5);

    //Creamos un Array nuevo para almacenar los elementos ahora Ordenados por la coordenada X en grupos de S*M
    int* Y = (int*) malloc(*N_cambiable * sizeof(int)*5) ;

    //Metemos para cada grupo de S*M elementos los rectangulos ordenados por su coordenada X
    for(int v = 0; v < S; v++){     // Dato curioso, aqui vamos a recorrer todo el array pero quedó con doble for por una estructura pasada XD
        for(int t = 0; t < S*M;t++){
            insertarOrdenadoX(Y,((v*S*M)+t)*5,Z,S,M, N); // Aqui no estoy seguro si a la dirección se le suman esos numeros por el sizeof(int) o como está
        }
    }

    // Array para Ordenar por Y--> actualmente están ordenados por X y se busca separarlos en grupos de S*M antes de ordenarlos por Y
    int* X = (int*) malloc(*N_cambiable * sizeof(int)*5) ;

    // Le insertamos los elementos ordenados por Y
    for(int a = 0; a < S; a++){
        for(int b = 0; b < S*M; b++){
            insertarOrdenadoY( X, ((a*S*M)+ b)*5, Y, a*S*M, (a*S*M) + (S*M), N);
        }
    }

    // Ahora estos son los nodos nuevos, toca guardarlos en un archivo
    //Ahora tenemos S*S nodos con M rectangulos c/u
    for(int a = 0; a < S*S; a++){
        // este for guarda todos los nodos de X 
        // Primero buscamos los 4 puntos de la hoja
        int Xmayor = 0;
        int Xmenor = 0;
        int Ymayor = 0; 
        int Ymenor = 0;
        for(int ptr = 0; ptr < M; ptr++){ // buscamos X e Y mayor y menor
            if(X[ a*(M+5)+(ptr*5)    ] < Xmenor){
                Xmenor = X[a*(M+5)+(ptr*5)];
            }
            if(X[ a*(M+5)+(ptr*5) + 1] < Ymenor){
                Ymenor = X[a*(M+5)+(ptr*5) + 1];
            }
            if(X[ a*(M+5)+(ptr*5) + 2] > Xmayor){
                Xmayor = X[a*(M+5)+(ptr*5) + 2];
            }
            if(X[ a*(M+5)+(ptr*5) + 3] > Ymayor){
                Ymayor = X[a*(M+5)+(ptr*5) + 3];
            }
        }
        // Teniendo los representates de la hoja toca escribir en el archivo los 4 puntos representantes y los punteros a los rectangulos
        int* nodo = (int*) malloc(sizeof(int)*(M+4)) ;
        int rectangulo = 4;
        nodo[0] = Xmenor;
        nodo[1] = Ymenor;
        nodo[2] = Xmayor;
        nodo[3] = Ymayor;
        while(rectangulo < M+4){
            nodo[rectangulo] = X[a*(M+5)+(rectangulo-4)+4];
            rectangulo++;
        }

        if(S*S == 1){
            fseek(archivoGuardado, 0 , SEEK_SET);
            fwrite(nodo, sizeof(int), 4+M, archivoGuardado);
            free(nodo);
            break;
        }
        fseek(archivoGuardado, *pos_Actual, SEEK_SET);
        fwrite(nodo, sizeof(int), 4+M, archivoGuardado);
        *pos_Actual = ftell(archivoGuardado);
        free(nodo);
        fclose(archivoGuardado);
    }
    *N_cambiable = *N_cambiable/M;

}