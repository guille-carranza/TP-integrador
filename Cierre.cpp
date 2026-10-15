#include <iostream>
#include <cstdio>

using namespace std;

//Tiene que ser identica a la struct de ventas.cpp
struct comanda {
    int id_mozo;
    int codigo_producto;
    int cantidad;
    float comision;
};

void armarNombreDiario(int dia, int mes, int anio, char nombre[]);
void armarNombreSemanal(int semana, int mes, char nombre[]);
int contarComandas(int dia, int mes, int anio);
void copiarComandas(int dia, int mes, int anio, comanda v[], int &pos);
void ordenarComandas(comanda v[], int n);

int main() {
    int semana, mes, anio;
    int diaInicio, diaFin;

    cout << "Ingrese el numero de semana: ";
    cin >> semana;

    cout << "Ingrese el mes (1-12): ";
    cin >> mes;

    cout << "Ingrese el anio: ";
    cin >> anio;

    cout << "Ingrese el primer dia que se va a cerrar: ";
    cin >> diaInicio;

    cout << "Ingrese el ultimo dia que se va a cerrar: ";
    cin >> diaFin;

    if(semana < 1 || mes < 1 || mes > 12 || anio < 1 || diaInicio < 1 || diaFin > 31 || diaInicio > diaFin) {
        cout << "Error: los datos ingresados no son validos."
             << endl;
        return 1;
    }

    // PRIMER RECORRIDO: contamos todas las comandas.
    int total = 0;

    for(int dia = diaInicio; dia <= diaFin; dia++)
    {
        total += contarComandas(dia, mes, anio);
    }

    cout << "Cantidad de comandas encontradas: "
         << total << endl;

    // Reservamos memoria para todas las comandas.
    comanda *v = NULL;

    if(total > 0)
    {
        v = new comanda[total];
    }

    // SEGUNDO RECORRIDO: copiamos todas las comandas.
    int pos = 0;

    for(int dia = diaInicio; dia <= diaFin; dia++)
    {
        copiarComandas(dia, mes, anio, v, pos);
    }

    // Ordenamos todas las comandas por numero de mozo.
    ordenarComandas(v, pos);

    // Creamos la planilla semanal.
    char nombreSemanal[40];
    armarNombreSemanal(semana, mes, nombreSemanal);

    FILE *semanal = fopen(nombreSemanal, "wb");

    if(semanal == NULL) {
        cout << "Error: no se pudo crear la planilla semanal."
             << endl;

        delete[] v;
        return 1;
    }

    if(pos > 0) {
        fwrite(v, sizeof(comanda), pos, semanal);
    }
    fclose(semanal);
    delete[] v;

    cout << "Cierre realizado correctamente." << endl;
    cout << "Archivo generado: " << nombreSemanal << endl;

    return 0;
}

// Construye el nombre de cada planilla diaria.
void armarNombreDiario(int dia, int mes, int anio, char nombre[]) {
    sprintf(nombre, "comandas_%02d-%02d-%04d.dat", dia, mes, anio);
}

// Construye el nombre de la planilla semanal.
void armarNombreSemanal(int semana, int mes, char nombre[]) {
    sprintf(nombre, "comandas_semana_s%d-%02d.dat",
            semana, mes);
}

// Cuenta los registros de una planilla diaria.
int contarComandas(int dia, int mes, int anio) {
    char nombre[40];

    armarNombreDiario(dia, mes, anio, nombre);

    FILE *archivo = fopen(nombre, "rb");

    if(archivo == NULL)
    {
        cout << "No existe la planilla " << nombre << endl;
        return 0;
    }

    fseek(archivo, 0, SEEK_END);

    int cantidad = ftell(archivo) / sizeof(comanda);

    fclose(archivo);

    return cantidad;
}

// Lee una planilla diaria y copia sus registros al arreglo
void copiarComandas(int dia, int mes, int anio, comanda v[], int &pos) {
    
    char nombre[40];

    armarNombreDiario(dia, mes, anio, nombre);

    FILE *archivo = fopen(nombre, "rb");

    if(archivo == NULL) {
        return;
    }

    comanda aux;

    while(fread(&aux, sizeof(comanda), 1, archivo) == 1) {
        v[pos] = aux;
        pos++;
    }

    fclose(archivo);
}

// Ordena las comandas por inserción, segun el numero de mozo
void ordenarComandas(comanda v[], int n) {
    for(int i = 1; i < n; i++) {
        comanda clave = v[i];
        int j = i - 1;

        while(j >= 0 && v[j].id_mozo > clave.id_mozo) {
            v[j + 1] = v[j];
            j--;
        }

        v[j + 1] = clave;
    }
}
