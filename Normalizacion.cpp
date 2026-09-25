#include<iostream>
#include<cstring>
using namespace std;

//Ordena lo que dejó el sistema viejo: arma la lista de mozos numerados con su comisión, separa las ventas por día (con el número de mozo y ordenadas por mozo) y deja el stock al día.
//Lee comandas_historicas.dat, inventario.dat y genera mozos.dat, comandas_dd-mm-aaaa.dat, inventario.dat

//Lista con mozo (cada uno una sola vez). Una clave como identificador y comision
//Tasa de comision: 10% de lo vendido

struct mozo{
    char nombre[50];
    char password[20];
    int id_mozo;
    float comision;
};

struct comanda{
    int id_mozo;
    int codigoProducto;
    int cantidad;
    float comision;
};

struct comandasHistoricas{
    char fecha[11]; //DD-MM-AAAA
    char nombreMozo[50]; //el nombre completo
    int codigoProducto;
    int cantidad;
    float comision;
};

struct Nodo{
    mozo info;
    Nodo* sig;
};

void copiarArchivo(Nodo*&inicio);
Nodo* crearNodo(mozo valor);
void insertarOrdenado(Nodo*&inicio, mozo valor);
void crearArchivo(Nodo*&inicio);
Nodo*buscarElemento(Nodo*inicio, char nombre[]);

int main()
{
    return 0;
}

Nodo*crearNodo(mozo valor)
{
    Nodo*nuevo=new Nodo();
    nuevo->info=valor;
    nuevo->sig=NULL;
    return nuevo;
}

void insertarOrdenado(Nodo*&inicio, mozo valor)
{
    Nodo*nuevo=crearNodo(valor);
    if(inicio==NULL || valor.id_mozo < inicio->info.id_mozo)
    {
        nuevo->sig=inicio;
        inicio=nuevo;
    }
    else if(inicio!=NULL)
    {
    Nodo*actual=inicio;
    while(actual->sig!=NULL && actual->sig->info.id_mozo < valor.id_mozo)
    {
        actual=actual->sig;
    }
    nuevo->sig=actual->sig;
    actual->sig=nuevo;
    }
}

void copiarArchivo(Nodo*&inicio){
int i=0;
FILE*archivo=fopen("comandas_historicas.dat", "rb");
comandasHistoricas arr;
if(archivo!=NULL)
{
while(fread(&arr, sizeof(comandasHistoricas),1,archivo)==1){
    Nodo*busco=buscarElemento(inicio, arr.nombreMozo);
   if(busco==NULL)
   {
    mozo aux;
    i++;
    strcpy(aux.nombre , arr.nombreMozo);
    aux.id_mozo=i;
    aux.comision=arr.comision;
    insertarOrdenado(inicio, aux);
   }
   else
   {
       busco->info.comision+=arr.comision;
   }
}
    fclose(archivo);
}
}

void crearArchivo(Nodo*&inicio)
{
    FILE*archivo=fopen("mozos.dat","wb");
    Nodo*actual=inicio;
    if(archivo!=NULL)
    {
    while(actual!=NULL)
    {
        fwrite(&actual->info, sizeof(mozo),1,archivo);
        Nodo*temp=actual;
        actual=actual->sig;
        delete temp;
    }

    fclose(archivo);
    inicio=NULL;
    }

}

Nodo*buscarElemento(Nodo*inicio, char nombre[])
{
    Nodo*actual=inicio;
    while(actual!=NULL)
    {
        if(strcmp(actual->info.nombre, nombre)==0)
        {
            return actual;
        }
        actual=actual->sig;
    }
    return NULL; //no se encontro
}
