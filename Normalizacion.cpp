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
void crearContrasena(mozo &valor);
//void mostrarArchivo(); //funcion opcional para verificar si se guardo bien el archivo mozos.dat

int main()
{
    Nodo*lista=NULL;
    copiarArchivo(lista);
    crearArchivo(lista);
    mostrarArchivo();
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
    crearContrasena(aux);
    insertarOrdenado(inicio, aux);
   }
   else
   {
       busco->info.comision+=arr.comision;
   }
}
    fclose(archivo);
}
else{
    cout<<"No se pudo abrir el archivo"<<endl;
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

void crearContrasena(mozo &valor)
{
    char contra[20];
    char*asigno=contra;

    for(int i=0;i<6;i++)
    {
        switch(i){

        case 0:
        asigno[i]=valor.nombre[i]; //Inicial del nombre
        break;

        case 1:
            {
            int j=0;
            while(valor.nombre[j+1]!='\0')
            {
                j++;
            }
            asigno[i]=valor.nombre[j]; //Ultima letra del nombre
            break;
            }
        case 2:
            asigno[i]='A'+(valor.id_mozo+3);//Caracter random
            break;

        case 3:
            asigno[i]='0'+(valor.id_mozo*3)%7; //de 0 a 6
            break;

        case 4:
            asigno[i]='0'+(13*valor.id_mozo)%9; //de 0 a 8
            break;

        case 5:
            asigno[i]=valor.nombre[1]; //2da letra del nombre
            break;
        case 6:
            contra[6]='\0';
            break;
    }
    }
    cout<<"La contrasena del mozo de ID "<<valor.id_mozo<<" es :"<<contra<<endl;
    for(int i=0; i<6; i++)
    {
        asigno[i]=asigno[i]+5; //desplazo 5 posiciones para guardarla en el registro
    }
    strcpy(valor.password,contra);
}

void mostrarArchivo()
{
    mozo aux;
    FILE*archivo=fopen("mozos.dat","rb");
    if(archivo==NULL)
    {
        cout<<"No se pudo abrir mozos.dat"<<endl;
    }
    else{
        while(fread(&aux, sizeof(mozo), 1, archivo)==1)
        {
            cout<<"Nombre: "<<aux.nombre<<endl;
            cout<<"Comision: "<<aux.comision<<endl;
            cout<<"ID: "<<aux.id_mozo<<endl;
            cout<<"Contrasena guardada en el archivo: "<<aux.password<<endl;
        }
    }
    fclose(archivo);
}

