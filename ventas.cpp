#include<iostream>
#include<cstdio>
#include<cstring>

using namespace std;

const float TASA_COMISION=0.10f; //10% de lo vendido
const int K=5; 

//porque lo leemos tal cual con fread. Si cambia el orden, leemos cualquier cosa.
struct mozo{
    char nombre[50];
    char password[20];
    int id_mozo;
    float comision;
};

struct comanda{
    int id_mozo;
    int codigo_producto;
    int cantidad;
    float comision;
};

struct producto{
    int codigo;
    char descripcion[50];
    float precio;
    int stock_actual;
};

int copiarCaracteres(char destino[], int pos, const char* origen);
void armarNombreArchivo(const char* fecha, char nombre[]);
void encriptar(char clave[]);
int validarMozo(int id_mozo, char clave_cifrada[]);
long buscarProducto(FILE* archivo, int codigo);
void registrarVenta(FILE* fComandas, FILE* fInventario, int id_mozo);
void ordenarPlanilla(const char* nombre);

int main()
{
    char fecha[11];
    cout<<"Ingrese la fecha del dia (DD-MM-AAAA): ";
    cin.getline(fecha, 11);

    char nombreArchivo[40];
    armarNombreArchivo(fecha, nombreArchivo);

    //"ab" agrega al final y, si la planilla del dia todavia no existe, la crea
    FILE* fComandas=fopen(nombreArchivo, "ab");
    if(fComandas==NULL)
    {
        cout<<"Error: no se pudo crear o abrir la planilla del dia. Revise que la fecha tenga el formato DD-MM-AAAA (con guiones)."<<endl;
        return 1;
    }

    //"rb+" porque leemos y ademas modificamos el stock sin rehacer el archivo
    FILE* fInventario=fopen("inventario.dat", "rb+");
    if(fInventario==NULL)
    {
        cout<<"Error: no se pudo abrir inventario.dat"<<endl;
        fclose(fComandas);
        return 1;
    }

    int opcion;
    do
    {
        cout<<endl<<"--- MENU PRINCIPAL ---"<<endl;
        cout<<"1 - Cargar una nueva venta"<<endl;
        cout<<"0 - Terminar el dia y cerrar"<<endl;
        cout<<"Opcion: ";
        cin>>opcion;

        if(opcion==1)
        {
            int id_mozo;
            char clave[20];
            int estado=1;

            //estado 1: hay que volver a pedir datos, 0: login ok, -1: el usuario cancelo
            while(estado==1)
            {
                cout<<"Numero de mozo (0 para volver al menu): ";
                cin>>id_mozo;

                if(id_mozo==0)
                {
                    estado=-1;
                }
                else
                {
                    cout<<"Clave: ";
                    cin>>clave;
                    encriptar(clave);
                    estado=validarMozo(id_mozo, clave);

                    if(estado==1)
                    {
                        cout<<"Error: ese mozo no existe."<<endl;
                    }
                    else if(estado==2)
                    {
                        cout<<"Error: clave incorrecta."<<endl;
                        estado=1;
                    }
                }
            }

            if(estado==0)
            {
                registrarVenta(fComandas, fInventario, id_mozo);
            }
        }
    }while(opcion!=0);

    fclose(fComandas);
    fclose(fInventario);

    ordenarPlanilla(nombreArchivo);
    cout<<"Planilla ordenada por mozo. Hasta manana"<<endl;

    return 0;
}

//Devuelve la posicion donde quedo parado, asi el siguiente pedazo arranca ahi.
int copiarCaracteres(char destino[], int pos, const char* origen)
{
    int i=0;
    while(origen[i]!='\0')
    {
        destino[pos]=origen[i];
        pos++;
        i++;
    }
    return pos;
}

void armarNombreArchivo(const char* fecha, char nombre[])
{
    strcpy(nombre, "comandas_"); 
    int pos=copiarCaracteres(nombre, 9, fecha);
    pos=copiarCaracteres(nombre, pos, ".dat");
    nombre[pos]='\0';
}

//Para el login hacemos lo mismo con lo que tipea el mozo y comparamos con lo guardado.
void encriptar(char clave[])
{
    int i=0;
    while(clave[i]!='\0')
    {
        clave[i]=clave[i]+K;
        i++;
    }
}

//Devuelve 0 si entra, 1 si el mozo no existe, 2 si la clave no coincide.
int validarMozo(int id_mozo, char clave_cifrada[])
{
    FILE* archivo=fopen("mozos.dat", "rb");
    if(archivo==NULL)
    {
        cout<<"Error: no se pudo abrir mozos.dat"<<endl;
        return 1;
    }

    fseek(archivo, 0, SEEK_END);
    long cantidad=ftell(archivo)/sizeof(mozo); //cuantos mozos hay en el archivo

    if(id_mozo<1 || id_mozo>cantidad)
    {
        fclose(archivo);
        return 1;
    }

    mozo aux;
    fseek(archivo, (long)(id_mozo-1)*sizeof(mozo), SEEK_SET);
    fread(&aux, sizeof(mozo), 1, archivo);
    fclose(archivo);

    if(strcmp(aux.password, clave_cifrada)==0)
    {
        return 0;
    }
    return 2;
}

// Busqueda binaria
long buscarProducto(FILE* archivo, int codigo)
{
    fseek(archivo, 0, SEEK_END);
    long n=ftell(archivo)/sizeof(producto);
    long pri=0, ult=n-1, pos=-1;
    producto p;

    while(pri<=ult && pos==-1)
    {
        long med=(pri+ult)/2;
        fseek(archivo, med*sizeof(producto), SEEK_SET);
        fread(&p, sizeof(producto), 1, archivo);
        if(p.codigo==codigo)
        {
            pos=med;
        }
        else if(codigo>p.codigo)
        {
            pri=med+1;
        }
        else
        {
            ult=med-1;
        }
    }
    return pos;
}

void registrarVenta(FILE* fComandas, FILE* fInventario, int id_mozo)
{
    int codigo, cantidad;
    cout<<"Ingrese el codigo de producto: ";
    cin>>codigo;
    cout<<"Ingrese la cantidad vendida: ";
    cin>>cantidad;

    if(cantidad<=0)
    {
        cout<<"La cantidad tiene que ser mayor a 0."<<endl;
        return;
    }

    long pos=buscarProducto(fInventario, codigo);
    if(pos==-1)
    {
        cout<<"Ese producto no existe en el inventario."<<endl;
        return;
    }

    producto p;
    fseek(fInventario, pos*sizeof(producto), SEEK_SET);
    fread(&p, sizeof(producto), 1, fInventario);

    if(p.stock_actual<cantidad)
    {
        cout<<"Sin stock suficiente. Solo quedan "<<p.stock_actual<<" unidades."<<endl;
        return;
    }

    //el fread de recien dejo el puntero en el producto SIGUIENTE; si grabamos ahi pisamos al vecino.
    p.stock_actual=p.stock_actual-cantidad;
    fseek(fInventario, pos*sizeof(producto), SEEK_SET);
    fwrite(&p, sizeof(producto), 1, fInventario);

    comanda c;
    c.id_mozo=id_mozo;
    c.codigo_producto=codigo;
    c.cantidad=cantidad;
    c.comision=p.precio*cantidad*TASA_COMISION;
    fwrite(&c, sizeof(comanda), 1, fComandas);

    cout<<"Venta registrada con exito. Comision generada: $"<<c.comision<<endl;
}

//Ordenr plantilla
void ordenarPlanilla(const char* nombre)
{
    FILE* archivo=fopen(nombre, "rb");
    if(archivo==NULL) return;

    fseek(archivo, 0, SEEK_END);
    int n=ftell(archivo)/sizeof(comanda);
    fseek(archivo, 0, SEEK_SET);

    if(n==0)
    {
        fclose(archivo);
        return;
    }

    comanda* v=new comanda[n];
    fread(v, sizeof(comanda), n, archivo);
    fclose(archivo);

    for(int i=1; i<n; i++)
    {
        comanda clave=v[i];
        int j=i-1;
        while(j>=0 && v[j].id_mozo>clave.id_mozo)
        {
            v[j+1]=v[j];
            j--;
        }
        v[j+1]=clave;
    }

    archivo=fopen(nombre, "wb");
    if(archivo!=NULL)
    {
        fwrite(v, sizeof(comanda), n, archivo);
        fclose(archivo);
    }
    delete[] v;
}
