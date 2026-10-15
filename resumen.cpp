// resumen.cpp
// TP "El buffet de Alberto" - AyED UTN FRBA
//
// Lee la planilla semanal comandas_semana_sX-mm.dat (ya ordenada por mozo)
// y muestra por pantalla:
//   - por cada mozo: cantidad de productos vendidos y comision a pagar
//   - al final: total de productos vendidos por el buffet en la semana
//
// Tecnica: CORTE DE CONTROL. Como el archivo esta ordenado por idMozo, todas
// las ventas de un mismo mozo son consecutivas. Se acumula mientras el idMozo
// no cambia; cuando cambia, se imprime el mozo anterior y se reinician los
// acumuladores. El ultimo mozo no tiene otro despues que dispare el corte,
// por eso se imprime aparte, al salir del while.

#include <stdio.h>
#include <iostream>
#include <iomanip>
using namespace std;

struct Comanda {
    int idMozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

void imprimirMozo(int idMozo, int cantidad, float comision) {
    cout << setw(6) << idMozo
         << setw(14) << cantidad
         << setw(18) << fixed << setprecision(2) << comision << endl;
}

int main() {
    int semana, mes;

    cout << "Semana del mes (1 a 5): ";
    cin >> semana;
    cout << "Mes (1 a 12): ";
    cin >> mes;

    // Arma el nombre: comandas_semana_sX-mm.dat (mes con dos digitos)
    char nombreArchivo[64];
    snprintf(nombreArchivo, sizeof(nombreArchivo),
             "comandas_semana_s%d-%02d.dat", semana, mes);

    FILE* f = fopen(nombreArchivo, "rb");
    if (f == NULL) {
        cout << "No se pudo abrir el archivo " << nombreArchivo << endl;
        return 1;
    }

    cout << endl << "RESUMEN DE CIERRE - " << nombreArchivo << endl << endl;
    cout << setw(6) << "Mozo"
         << setw(14) << "Productos"
         << setw(18) << "Comision ($)" << endl;
    cout << "------------------------------------------" << endl;

    Comanda c;
    bool hayDatos = false;      // sirve para detectar archivo vacio
    int mozoActual = 0;
    int cantMozo = 0;           // productos del mozo actual
    float comisionMozo = 0;     // comision del mozo actual
    int totalBuffet = 0;        // productos de todo el buffet

    while (fread(&c, sizeof(Comanda), 1, f) == 1) {
        if (!hayDatos) {
            // primera venta del archivo: arranca el primer mozo
            hayDatos = true;
            mozoActual = c.idMozo;
        } else if (c.idMozo != mozoActual) {
            // corte de control: cambio de mozo
            imprimirMozo(mozoActual, cantMozo, comisionMozo);
            mozoActual = c.idMozo;
            cantMozo = 0;
            comisionMozo = 0;
        }

        cantMozo += c.cantidad;
        comisionMozo += c.comision;
        totalBuffet += c.cantidad;
    }

    fclose(f);

    if (!hayDatos) {
        cout << "La planilla semanal no tiene ventas." << endl;
        return 0;
    }

    // el ultimo mozo no dispara corte dentro del while: se imprime aca
    imprimirMozo(mozoActual, cantMozo, comisionMozo);

    cout << "------------------------------------------" << endl;
    cout << "Total de productos vendidos en la semana: " << totalBuffet << endl;

    return 0;
}
