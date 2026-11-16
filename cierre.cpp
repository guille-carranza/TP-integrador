#include <iostream>
#include <cstdio>

using namespace std;

//tiene que ser identica a la estructura de ventas.cpp
struct comanda {
    int id_mozo;
    int codigo_producto;
    int cantidad;
    float comision;
};

void armarNombreDiario(int dia, int mes, int anio, char nombre[]);
void armarNombreSemanal(int semana, int mes, char nombre[]);

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

    if(semana < 1 || mes < 1 || mes > 12 || anio < 1 || diaInicio < 1 || diaInicio > 31 
        ||diaFin < 1 || diaFin > 31 || diaInicio > diaFin || diaFin - diaInicio > 6)
    {
        cout << "Error: los datos ingresados no son validos." << endl;
        return 1;
    }

    int cantidadDias = diaFin - diaInicio + 1;

    //arreglos para manejar los archivos diarios.
    FILE *archivos[7];
    comanda actual[7];
    int leido[7];

    //abrimos las planillas diarias y leemos el primer registro de cada una.
    for(int i = 0; i < cantidadDias; i++) {
        char nombre[40];

        armarNombreDiario(diaInicio + i, mes, anio, nombre);

        archivos[i] = fopen(nombre, "rb");

        if(archivos[i] == NULL) {
            cout << "No se pudo abrir " << nombre << endl;
            leido[i] = 0;
        }
        else
        {
            leido[i] = fread(&actual[i],
                             sizeof(comanda), 1, archivos[i]);
        }
    }

    //creamos la planilla semanal.
    char nombreSemanal[40];

    armarNombreSemanal(semana, mes, nombreSemanal);

    FILE *semanal = fopen(nombreSemanal, "wb");

    if(semanal == NULL) {
        cout << "Error: no se pudo crear la planilla semanal."
             << endl;

        for(int i = 0; i < cantidadDias; i++)
        {
            if(archivos[i] != NULL)
            {
                fclose(archivos[i]);
            }
        }

        return 1;
    }

    // apareo: buscamos siempre el registro con menor numero de mozo entre los registros actuales.
    while(true) {
        int menor = -1;

        for(int i = 0; i < cantidadDias; i++) {
            if(leido[i] == 1) {
                if(menor == -1 ||actual[i].id_mozo < actual[menor].id_mozo){
                    menor = i;
                }
            }
        }

        //si no quedan registros en ningun archivo, terminamos el apareo.
        if(menor == -1) {
            break;
        }

        //escribimos el registro de menor mozo.
        if(fwrite(&actual[menor], sizeof(comanda), 1, semanal) != 1) {
            cout << "Error al escribir la planilla semanal."
                 << endl;

            fclose(semanal);

            for(int i = 0; i < cantidadDias; i++) {
                if(archivos[i] != NULL) {
                    fclose(archivos[i]);
                }
            }

            return 1;
        }

        // avanzamos solamente el archivo del registro que acabamos de escribir.
        leido[menor] = fread(&actual[menor], sizeof(comanda), 1, archivos[menor]);
    }

    // cerramos todos los archivos diarios.
    for(int i = 0; i < cantidadDias; i++) {
        if(archivos[i] != NULL) {
            fclose(archivos[i]);
        }
    }

    fclose(semanal);

    cout << "Cierre realizado correctamente." << endl;
    cout << "Archivo generado: " << nombreSemanal << endl;

    return 0;
}

// construye el nombre de cada planilla diaria.
void armarNombreDiario(int dia, int mes, int anio, char nombre[]) {
    sprintf(nombre, "comandas_%02d-%02d-%04d.dat",
            dia, mes, anio);
}

//construye el nombre de la planilla semanal.
void armarNombreSemanal(int semana, int mes, char nombre[]) {
    sprintf(nombre, "comandas_semana_s%d-%02d.dat", semana, mes);
}
 
