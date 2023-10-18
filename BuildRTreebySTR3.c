#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "printIntsFromFile.c"
#include "generateRectangleFile.c"

int buildRTreebySTR(char *dirArchivoEntrada, char *dirArchivoSalida, int M, long N);
void insertarOrdenadoX(long* Objetivo, int ptr_obj, long* Pozo, long S, int M, long N);
void insertarOrdenadoY(long* Objetivo, int ptr_obj, long* Pozo,int inicioPozo, int finalPozo, long N);
void guardarHoja(long* Objetivo, int inicio, int M, FILE* archivoGuardado, long* pos_Actual);
int CrearNodos(char* dirArchivoSalida, long* pos_Busqueda, long* pos_Actual, long* N_cambiable,int M, long N);

int buildRTreebySTR(char *dirArchivoEntrada, char *dirArchivoSalida, int M, long N) {

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Abrimos el archivo R de rectangulos y lo pasamos a un archivo intermedio añadiendo el indice que tienen como un parametro mas//
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FILE *archivoEntrada, *archivoIntermedio;
    char *nombreArchivoIntermedio = "intermedio.bin";

    // Abrir el archivo de entrada en modo lectura binaria
    archivoEntrada = fopen(dirArchivoEntrada, "rb");
    if (archivoEntrada == NULL) {
        perror("Error al abrir el archivo de entrada");
        return 1;
    }

    // Abrir el archivo de intermedio en modo escritura binaria
    archivoIntermedio = fopen(nombreArchivoIntermedio, "wb");
    if (archivoIntermedio == NULL) {
        perror("Error al abrir el archivo de salida");
        fclose(archivoEntrada);
        return 1;
    }

    long buffer[4];  // Buffer para almacenar los enteros
    long indice = -1;
    // Leer y escribir enteros en grupos de TAMANO_GRUPO
    while (fread(buffer, sizeof(long), 4, archivoEntrada) == 4) {
        fwrite(buffer, sizeof(long), 4, archivoIntermedio);
        fwrite(&indice,sizeof(long),1,archivoIntermedio);
        indice--;
    }

    // Cerrar los archivos
    fclose(archivoEntrada);
    fclose(archivoIntermedio);

    //////////////////////////////////////////////////////////////
    /////Metemos los rectangulos de 5 elementos a un array Z /////
    //////////////////////////////////////////////////////////////

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
    size_t num_longs = file_size / sizeof(long);
    //Cte S
    long S = pow((N/M),0.5); 

    printf("number of longs: %ld\n", num_longs);
    printf("number of rects: %ld\n", N);
    printf("valor de S: %ld\n", S);
    printf("----------VALOR DE SSM=%ld----------\n", S*S*M);

    // Allocate memory for the array using malloc
    long *Z = (long *)malloc(num_longs * sizeof(long));

    // Check if malloc was successful
    if (Z == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        fclose(archivo);
        return 1;
    }

    // Read integers from the file into the array
    fread(Z, sizeof(long), num_longs, archivo);

    printf("--------------------Longs from the array Z HOJAS:-------------------- \n");
    for (int i = 0; i < N; i++) {
        printf("RECTANGULO: %ld,%ld,%ld,%ld//%ld\n",Z[i*5+0],Z[i*5+1],Z[i*5+2],Z[i*5+3],Z[i*5+4]);
    }

    // Close the file
    fclose(archivo);

    /////////////////////////////////////////////////////////////////
    // Ordenamos por la coordenada X el array Z y lo guardamos en Y//
    /////////////////////////////////////////////////////////////////

    //Creamos un Array nuevo para almacenar los elementos ahora Ordenados por la coordenada X en grupos de S*M
    long* Y = (long*) malloc(N * sizeof(long)*5) ;

    //Metemos para cada grupo de S*M elementos los rectangulos ordenados por su coordenada X
    printf("---------------EMPIEZA LA INSERCIÓN SEGÚN X----------------\n");
    for(int v = 0; v < N; v++){
        printf("----------ITERACIÓN : %d----------\n",v+1);  
        insertarOrdenadoX(Y,v*5,Z,S,M,N);
    } 

    printf("--------------------Longs from the array Y HOJAS: order by X-------------------- \n");
    for (int i = 0; i < N; i++) {
        long x1 = Y[i * 5];
        long y1 = Y[i * 5 + 1];
        long x2 = Y[i * 5 + 2];
        long y2 = Y[i * 5 + 3];
        long ind = Y[i * 5 + 4];
        long centerX = (x1 + x2) / 2;
        long centerY = (y1 + y2) / 2;
        printf("RECTANGULO: %ld,%ld,%ld,%ld//%ld  - Center: (%ld, %ld) \n",Y[i*5+0],Y[i*5+1],Y[i*5+2],Y[i*5+3],Y[i*5+4],centerX,centerY);
    }

    /////////////////////////////////////////////////////////////////
    // Ordenamos por la coordenada Y el array Y y lo guardamos en X//
    /////////////////////////////////////////////////////////////////

    // Array para Ordenar por Y--> actualmente están ordenados por X y se busca separarlos en grupos de S*M antes de ordenarlos por Y
    long* X = (long*) malloc(N * sizeof(long)*5) ;

    // Le insertamos los elementos ordenados por Y
    for(int a = 0; a < S; a++){ 
        for(int b = 0; b < S*M; b++){
            insertarOrdenadoY( X, ((a*S*M)+ b)*5, Y, a*S*M, (a*S*M) + (S*M), N);
        }
    }
    for(int c = 0; c < N-S*S*M;c++){
        insertarOrdenadoY(X,((S*S*M)+c)*5,Y,S*S*M,N,N);
    }

    printf("--------------------Longs from array X: order by Y, groups of %ld:-------------------- \n",S*M);
    for (int i = 0; i < N; ++i) {
        // Extracting coordinates for the current rectangle
        long x1 = X[i * 5];
        long y1 = X[i * 5 + 1];
        long x2 = X[i * 5 + 2];
        long y2 = X[i * 5 + 3];
        long ind = X[i * 5 + 4];

        // Calculating center coordinates
        long centerX = (x1 + x2) / 2;
        long centerY = (y1 + y2) / 2;

        // Printing the information
        printf("Rectangle %d: (%ld, %ld, %ld, %ld) - Center: (%ld, %ld)\n", i + 1, x1, y1, x2, y2, centerX, centerY);
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
    *pos_Busqueda = sizeof(long)*(4+M);

    //////////////////////////////////////////////////////////
    //Ahora teniendo ordenado por X e Y, guardamos las hojas//
    //////////////////////////////////////////////////////////
    printf("--------------------------------------------------\n");
    fseek(archivoGuardado, *pos_Busqueda, SEEK_SET);
    for(int a = 0; a <= S*S; a++){
        printf("Insertada Hoja %d\n",a+1);
        guardarHoja(X, a, M, archivoGuardado,pos_Actual); // Esta función debería guardar la hoja con los punteros a los M rectangulos
    }
    // Tarea Terminada, Cerramos el archivo 
    fclose(archivoGuardado);

    // Chequeamos que se hayan guardado bien
    printf("--------------------INFO GUARDADA EN HOJAS--------------------\n");
    FILE *fileXD = fopen(dirArchivoSalida, "rb");

    // Check if the file was opened successfully
    if (fileXD == NULL) {
        fprintf(stderr, "Error opening file: %s\n", dirArchivoSalida);
        return 1; // Return an error code
    }

    // Read long integers from the file and print them
    long current_long;
    while (fread(&current_long, sizeof(long), 1, fileXD) == 1) {
        printf("%ld ", current_long);
    }

    // Close the file
    fclose(fileXD);

    printf("\n");
    long NumNodosActuales = N;
    if(NumNodosActuales%M > 0) {
        NumNodosActuales = (NumNodosActuales/M)+1;
    }
    else {
        NumNodosActuales =  NumNodosActuales/M;
    }
    free(X);
    free(Y);
    free(Z);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Teniendo los nodos hoja guardados en un archivo a partir de ellos creamos nodos y los guardamos hasta tener una raiz//
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    while(NumNodosActuales > 1){
        printf("-----------------------Valor Nodos Actuales PRE CREAR NODOS: %ld-----------------------------------\n",NumNodosActuales);
        CrearNodos(dirArchivoSalida,pos_Busqueda,pos_Actual,&NumNodosActuales,M, N);
        printf("-----------------------Valor Nodos Actuales POST CREAR NODOS: %ld-----------------------------------\n", NumNodosActuales);
    }

    return 0;
}

void insertarOrdenadoX(long* Objetivo, int ptr_obj, long* Pozo, long S, int M, long N) {
    //Esta función agrega a Objetivo el nodo de menor valor medio de la coordenada X, del Array Pozo y lo elimina de este (ahora vale -1 y no va a contar para los siguientes usos)
    int pos = 999999999; // posición del elegido
    int xd = 999999999; // menor valor medio de X
    for(int a = 0; a < N*5 ;a += 5){ // buscamos el elemento de menor centro en X para agregar a Objetivo
        if(Pozo[a] > -1){
            printf("RECTANGULO: %ld, %ld, %ld, %ld\n", Pozo[a], Pozo[a+1], Pozo[a+2], Pozo[a+3]);
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

void insertarOrdenadoY(long* Objetivo, int ptr_obj, long* Pozo,int inicioPozo, int finalPozo, long N) {
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

void guardarHoja(long* Objetivo, int inicio, int M, FILE* archivoGuardado, long* pos_Actual) {
    // esta función guarda todas las hojas de Objetivo con rectangulos desde inicio hasta inicio + M en archivoGuardado
    //Recordar que Objetivo es el Array X que contiene rectangulos representados por 4 puntos y su dirección a R

    // Primero buscamos los 4 puntos representantes de la hoja
    int Xmayor = 0;
    int Xmenor = 999999999;
    int Ymayor = 0; 
    int Ymenor = 999999999;
    for(int ptr = 0; ptr < M; ptr++){ // buscamos X e Y mayor y menor
        if(Objetivo[ inicio*(M*5)+(ptr*5)    ] < Xmenor){
            Xmenor = Objetivo[inicio*(M*5)+(ptr*5)];
        }
        if(Objetivo[ inicio*(M*5)+(ptr*5) + 1] < Ymenor){
            Ymenor = Objetivo[inicio*(M*5)+(ptr*5) + 1];
        }
        if(Objetivo[ inicio*(M*5)+(ptr*5) + 2] > Xmayor){
            Xmayor = Objetivo[inicio*(M*5)+(ptr*5) + 2];
        }
        if(Objetivo[ inicio*(M*5)+(ptr*5) + 3] > Ymayor){
            Ymayor = Objetivo[inicio*(M*5)+(ptr*5) + 3];
        }
    }
    // Teniendo los representates de la hoja toca escribir en el archivo los 4 puntos representantes y los punteros a los rectangulos que serán los hijos
    long* hoja = (long*) malloc(sizeof(long)*(M+4)) ;
    int pos_hijos = 4;
    hoja[0] = Xmenor;
    hoja[1] = Ymenor;
    hoja[2] = Xmayor;
    hoja[3] = Ymayor;
    while(pos_hijos < M+4){
        hoja[pos_hijos] = Objetivo[inicio*M*5 + ((pos_hijos-4)*5)+4]; // pos_hijos-4 -> numero del hijo que estamos mirando
        pos_hijos++;                                                  // multiplicamos por 5 por la cantidad para posicionarnos en el primer elemento del rectangulo
    }                                                                 // sumamos 4 para pasar del x1 -> puntero
    fwrite(hoja, sizeof(long), 4+M, archivoGuardado);
    *pos_Actual = ftell(archivoGuardado);
    free(hoja);
}

int CrearNodos(char* dirArchivoSalida, long* pos_Busqueda, long* pos_Actual, long* N_cambiable,int M, long N) { // el N_cambiable es el nuevo N 
    
    FILE* archivoGuardado = fopen(dirArchivoSalida, "rb");
    FILE* archivoIntermedio2;
    char* nombreArchivoIntermedio2 = "intermedio2.bin";

    // Abrir el archivo intermedio en modo escritura binaria
    archivoIntermedio2 = fopen(nombreArchivoIntermedio2, "wb");
    if (archivoIntermedio2 == NULL) {
        perror("Error al abrir el archivo de salida");
        fclose(archivoIntermedio2);
        return 1;
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Escribimos los N_cambiable nodos existentes en el nivel posterior ya creados en un archivo intermedio//
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    long pos_Busqueda2 = *pos_Busqueda;

    long buffer2[4];  // Buffer para almacenar los longs
    printf("--------------------Posicion de busqueda Nodos Anteriores: %ld-------------------- \n", *pos_Busqueda);
    fseek(archivoGuardado,*pos_Busqueda,SEEK_SET); // punteroa en posicion de busqueda
    int NodosLeidos = 0;
    while (NodosLeidos < *N_cambiable) { // Leemos tantos nodos como los Nodos del nivel posterior ya creados
        fread(buffer2, sizeof(long), 4, archivoGuardado); // leemos 4 numeros
        fwrite(buffer2, sizeof(long), 4, archivoIntermedio2); // escribimos en el archivo intermedio los 4 longs
        fwrite(&pos_Busqueda2,sizeof(long),1,archivoIntermedio2); // escribimos como quinto elemento el puntero al nodo
        pos_Busqueda2+=(4+M)*sizeof(long);// la posición de busqueda se mueve M longs hasta el siguiente nodo
        fseek(archivoGuardado, sizeof(long)*M,SEEK_CUR); // movemos el cursor del archivo con los nodos guardados M longs
        NodosLeidos++; 
        printf("BUFFFER:");
        for (int i = 0; i < 4; i++) {
            printf("%ld--", buffer2[i]);
        }
        printf("\n");
        
    }
    *pos_Busqueda = *pos_Busqueda + (4+M)*sizeof(long)*(*N_cambiable);

    // Cerrar los archivos
    fclose(archivoGuardado);
    fclose(archivoIntermedio2);


    FILE *archivo = fopen(nombreArchivoIntermedio2, "rb");

    // Check if the file was opened successfully
    if (archivo == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    // Get the size of the file
    fseek(archivo, 0, SEEK_END);
    long file_size = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);

    // Calculate the number of longs in the file
    size_t num_integers = file_size / sizeof(long);

    // Allocate memory for the array using malloc
    long *Z = (long *)malloc(num_integers * sizeof(long));

    // Check if malloc was successful
    if (Z == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        fclose(archivo);
        return 1;
    }


    ///////////////////////////////////////////////////////////////
    //Leemos todos los nodos del archivo intermedio en un array Z// 
    ///////////////////////////////////////////////////////////////

    // Read integers from the file into the array
    fseek(archivo, 0, SEEK_SET);
    fread(Z, sizeof(long), num_integers, archivo);


    printf("--------------------Longs from the array Z NODOS:-------------------- \n");
    for (int i = 0; i < *N_cambiable; i++) {
        printf("RECTANGULO: %ld,%ld,%ld,%ld//%ld\n",Z[i*5+0],Z[i*5+1],Z[i*5+2],Z[i*5+3],Z[i*5+4]);
    }

    // Close the file
    fclose(archivo);

    printf("--------------------Numero Actual de Longs: %ld--------------------\n", num_integers);
    printf("--------------------N_cambiable: %ld--------------------\n", *N_cambiable);

    long S = pow(*N_cambiable/M,0.5);
    //Creamos un Array nuevo para almacenar los elementos ahora Ordenados por la coordenada X en grupos de S*M
    long* Y = (long*) malloc(num_integers*sizeof(long));

    /////////////////////////////////////////////////////////////////
    // Ordenamos por la coordenada X el array Z y lo guardamos en Y//
    /////////////////////////////////////////////////////////////////

    //Metemos para cada grupo de S*M elementos los rectangulos ordenados por su coordenada X
    for(int v = 0; v < *N_cambiable; v++){     // Dato curioso, aqui vamos a recorrer todo el array pero quedó con doble for por una estructura pasada XD
        insertarOrdenadoX(Y,v*5,Z,S,M, *N_cambiable); // Aqui no estoy seguro si a la dirección se le suman esos numeros por el sizeof(int) o como está
        printf("--------------------ITERACIÓN: %d--------------------\n", v);
    }

    printf("--------------------Longs from the array Y NODOS: order by X-------------------- \n");
    for (int i = 0; i < *N_cambiable; i++) {
        long x1 = Y[i * 5];
        long y1 = Y[i * 5 + 1];
        long x2 = Y[i * 5 + 2];
        long y2 = Y[i * 5 + 3];
        long ind = Y[i * 5 + 4];
        long centerX = (x1 + x2) / 2;
        long centerY = (y1 + y2) / 2;
        printf("RECTANGULO: %ld,%ld,%ld,%ld//%ld  - Center: (%ld, %ld) \n",Y[i*5+0],Y[i*5+1],Y[i*5+2],Y[i*5+3],Y[i*5+4],centerX,centerY);
    }

    // Array para Ordenar por Y--> actualmente están ordenados por X y se busca separarlos en grupos de S*M antes de ordenarlos por Y
    long* X = (long*) malloc(num_integers* sizeof(long));

    /////////////////////////////////////////////////////////////////
    // Ordenamos por la coordenada Y el array Y y lo guardamos en X//
    /////////////////////////////////////////////////////////////////

    // Le insertamos los elementos ordenados por Y
    for(int a = 0; a < S; a++){ 
        for(int b = 0; b < S*M; b++){
            insertarOrdenadoY( X, ((a*S*M)+ b)*5, Y, a*S*M, (a*S*M) + (S*M), *N_cambiable);
        }
    }

    for(int c = 0; c < *N_cambiable-S*S*M;c++){
        insertarOrdenadoY(X,((S*S*M)+c)*5,Y,S*S*M,*N_cambiable,*N_cambiable);
    }

    printf("--------------------Longs from the array X NODOS: order by Y-------------------- \n");
    for (int i = 0; i < *N_cambiable; i++) {
        long x1 = X[i * 5];
        long y1 = X[i * 5 + 1];
        long x2 = X[i * 5 + 2];
        long y2 = X[i * 5 + 3];
        long ind = X[i * 5 + 4];
        long centerX = (x1 + x2) / 2;
        long centerY = (y1 + y2) / 2;
        printf("RECTANGULO: %ld,%ld,%ld,%ld//%ld  - Center: (%ld, %ld) \n",X[i*5+0],X[i*5+1],X[i*5+2],X[i*5+3],X[i*5+4],centerX,centerY);
    }

    // Ahora estos son los nodos nuevos, toca guardarlos en un archivo
    //Ahora tenemos S*S nodos con M rectangulos c/u
    int N_especial;

    if(*N_cambiable%M > 0) {
        N_especial = (*N_cambiable/M)+1;
    }
    else {
        N_especial =  *N_cambiable/M;
    }

    archivoGuardado = fopen(dirArchivoSalida, "wb");
    for(int a = 0; a < N_especial; a++){
        // este for guarda todos los nodos de X 
        // Primero buscamos los 4 puntos de la hoja
        long Xmayor = 0;
        long Xmenor = 999999999;
        long Ymayor = 0; 
        long Ymenor = 999999999;
        if(a == N_especial-1){ // si estamos en el ultimo Nodo a guardar
            if(*N_cambiable%M > 0){ // y los nodos actuales  al dividirse en M grupos dejan al ultimo grupo con menos nodos
                for(int ptr = 0; ptr < *N_cambiable%M; ptr++){ // buscamos X e Y mayor y menor
                    if(X[ a*(M*5)+(ptr*5)    ] < Xmenor){
                        Xmenor = X[a*(M*5)+(ptr*5)];
                    }
                    if(X[ a*(M*5)+(ptr*5) + 1] < Ymenor){
                        Ymenor = X[a*(M*5)+(ptr*5) + 1];
                    }
                    if(X[ a*(M*5)+(ptr*5) + 2] > Xmayor){
                        Xmayor = X[a*(M*5)+(ptr*5) + 2];
                    }
                    if(X[ a*(M*5)+(ptr*5) + 3] > Ymayor){
                        Ymayor = X[a*(M*5)+(ptr*5) + 3];
                    }
                }
            }
            if(N_especial == 1){
                for(int ptr = 0; ptr < M-*N_cambiable%M; ptr++){ // buscamos X e Y mayor y menor
                    if(X[ a*(M*5)+(ptr*5)    ] < Xmenor){
                        Xmenor = X[a*(M*5)+(ptr*5)];
                    }
                    if(X[ a*(M*5)+(ptr*5) + 1] < Ymenor){
                        Ymenor = X[a*(M*5)+(ptr*5) + 1];
                    }
                    if(X[ a*(M*5)+(ptr*5) + 2] > Xmayor){
                        Xmayor = X[a*(M*5)+(ptr*5) + 2];
                    }
                    if(X[ a*(M*5)+(ptr*5) + 3] > Ymayor){
                        Ymayor = X[a*(M*5)+(ptr*5) + 3];
                    }
                }
            }
        }
        else{
            for(int ptr = 0; ptr < M; ptr++){ // buscamos X e Y mayor y menor
                if(X[ a*(M*5)+(ptr*5)    ] < Xmenor){
                    Xmenor = X[a*(M*5)+(ptr*5)];
                }
                if(X[ a*(M*5)+(ptr*5) + 1] < Ymenor){
                    Ymenor = X[a*(M*5)+(ptr*5) + 1];
                }
                if(X[ a*(M*5)+(ptr*5) + 2] > Xmayor){
                    Xmayor = X[a*(M*5)+(ptr*5) + 2];
                }
                if(X[ a*(M*5)+(ptr*5) + 3] > Ymayor){
                    Ymayor = X[a*(M*5)+(ptr*5) + 3];
                }
            }
        }
        printf("RECTANGULO A GUARDAR: %ld,%ld,%ld,%ld \n",Xmenor,Ymenor,Xmayor,Ymayor);
        // Teniendo los representates de la hoja toca escribir en el archivo los 4 puntos representantes y los punteros a los rectangulos
        long* nodo = (long*) malloc(sizeof(long)*(M+4)) ;
        int rectangulo = 4;
        nodo[0] = Xmenor;
        nodo[1] = Ymenor;
        nodo[2] = Xmayor;
        nodo[3] = Ymayor;

        if(a == N_especial-1 && *N_cambiable%M > 0){
                while(rectangulo < (*N_cambiable%M)+4){
                    nodo[rectangulo] = X[(a*5*M)+(rectangulo-4)*5+4];
                    rectangulo++;
                }
        }
        else{
            while(rectangulo < M+4){
                nodo[rectangulo] = X[(a*5*M)+(rectangulo-4)*5+4]; // a*M*5 --> Numero de longs que hay que saltarse para saltarnos M rectangulos
                rectangulo++;                                      
            }                                                       
        }

        if(*N_cambiable <= M){
            fseek(archivoGuardado, 0 , SEEK_SET);
            fwrite(nodo, sizeof(long), 4+M, archivoGuardado);
            free(nodo);
        }
        else{
            printf("----------------------------LA POSICION ACTUAL ES : %ld-----------------------------\n",*pos_Actual);
            fseek(archivoGuardado, *pos_Actual, SEEK_SET);
            fwrite(nodo, sizeof(long), 4+M, archivoGuardado);
            *pos_Actual = ftell(archivoGuardado);
            free(nodo);
        }
    }
    fclose(archivoGuardado);

    if(*N_cambiable%M > 0) {
        *N_cambiable = (*N_cambiable/M)+1;
    }
    else {
        *N_cambiable =  *N_cambiable/M;
    }

    return 0;

}

int main() {
    int M = 4;
    int n = 20;

    printf("Generation of rectangles for testing...\n");
    printf("--------------\n");
    generateRectangleFile("rect_test.bin", n, 0, 100, 0, 5);
    printIntsFromFile("rect_test.bin", 4);

    char filename_m3[] = "tree_3.bin";
    buildRTreebySTR("rect_test.bin", filename_m3, M, n);

    printf("The tree has been successfully created.\n");
    printIntsFromFile(filename_m3, M+4);    

    return 0;
}