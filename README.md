# TP Integrador — Buffet Albert Einstein

Trabajo Práctico grupal de **Algoritmos y Estructuras de Datos** (UTN FRBA) —
unidad de **archivos binarios en C/C++**.

> **Grupo:** _7_\
> **Integrantes:** Guillermo Javier Carranza, guille-carranza\
> Ruben Alfredo Escobar, ruben-escobar\
> Cristian Gabriel Serpa, crisserpa\
> Mirko Quevedo Urtiaga, mirkcr\
> Thomas Baron, tbaron05

La consigna completa está en **`enunciado.pdf`**.

## Requisitos

- Compilador de C++ (`g++`, C++11 o superior).
- No se usan librerías externas: solo la API estándar de C (`fopen`, `fread`,
  `fwrite`, `fseek`, `fclose`). El cifrado de claves está hecho a mano.

Instalación de `g++`:

- **Linux:** `sudo apt install g++`
- **Mac:** `xcode-select --install`
- **Windows:** MinGW-w64 o un IDE que lo incluya (por ejemplo Code::Blocks).

## Estructura del repo

```
.
├── enunciado.pdf              # la consigna
├── datos/                     # archivos PROVISTOS por la cátedra (no se tocan)
│   ├── comandas_historicas.dat
│   ├── inventario.dat
│   └── dump_datos_de_prueba.txt
├── normalizacion.cpp
├── ventas.cpp
├── cierre.cpp
├── resumen.cpp
├── .gitignore
└── README.md
```

Cada programa tiene su propio `main` (no hay `main.cpp`).

## Cómo compilar y correr

Los programas leen y escriben `.dat` en la carpeta donde se ejecutan. Los de
`datos/` son los originales: hay que copiarlos a la carpeta de trabajo.

```bash
cp datos/comandas_historicas.dat datos/inventario.dat .
```

En Windows (sin Git Bash): `copy datos\*.dat .`

Compilación:

```bash
g++ -O2 -o normalizacion normalizacion.cpp
g++ -O2 -o ventas        ventas.cpp
g++ -O2 -o cierre        cierre.cpp
g++ -O2 -o resumen       resumen.cpp
```

Orden de ejecución (cada programa deja los archivos que usa el siguiente):

| Orden | Programa | Lee | Genera |
|---|---|---|---|
| 1 | `./normalizacion` | `comandas_historicas.dat`, `inventario.dat` | `mozos.dat`, `comandas_dd-mm-aaaa.dat` (uno por día), `inventario.dat` actualizado |
| 2 | `./ventas` | `mozos.dat`, `inventario.dat` | agrega a `comandas_dd-mm-aaaa.dat` (o la crea) y actualiza `inventario.dat` |
| 3 | `./cierre` | `comandas_dd-mm-aaaa.dat` | `comandas_semana_sX-mm.dat` |
| 4 | `./resumen` | `comandas_semana_sX-mm.dat` | reporte por pantalla |

**Importante:** `normalizacion` descuenta del inventario todas las ventas
históricas cada vez que se ejecuta. Si se corre dos veces sobre el mismo
`inventario.dat`, el stock se descuenta dos veces. Antes de repetir la corrida,
volver a copiar `inventario.dat` desde `datos/`.

`normalizacion` muestra por pantalla la clave inicial de cada mozo (en texto
plano). Es la única forma de conocerla, porque en `mozos.dat` queda cifrada.

## Quién hizo cada parte

| Programa | Integrante(s) |
|---|---|
| `normalizacion.cpp` — lista de mozos y comisión acumulada | guille-carranza |
| `normalizacion.cpp` — planillas por día y actualización de stock | ruben-escobar |
| `ventas.cpp` | crisserpa |
| `cierre.cpp` | mirkcr |
| `resumen.cpp` | tbaron05 |

## Cómo resolvimos las partes clave

### Claves (login y cifrado)

Las claves no se guardan en texto plano. Usamos un **corrimiento**: a cada
carácter se le suma un número fijo `K = 5`.

- `normalizacion` genera una clave inicial de 6 caracteres para cada mozo a
  partir de su nombre y su número, y la guarda ya desplazada.
- `ventas` pide la clave, le aplica la misma cuenta (`+5` a cada carácter) y
  compara con `strcmp` contra lo guardado en `mozos.dat`.
- Ejemplo con la clave `sol`: `s`(115)+5 → `x`, `o`(111)+5 → `t`,
  `l`(108)+5 → `q`; se guarda `xtq`. Si el mozo escribe `sol`, se cifra a `xtq`
  y coincide.
- La cuenta **se puede deshacer** restando `K` a cada carácter. A diferencia de
  dividir por dos, que pierde información (16 y 17 dan lo mismo), sumar un
  número fijo es una función uno a uno.

### Lista de mozos

Se recorre `comandas_historicas.dat` con `while (fread(...) == 1)`. Cada nombre
nuevo recibe un número correlativo (1, 2, 3...) y se agrega a una lista enlazada
ordenada por número. Si el nombre ya estaba, solo se le suma la comisión. Al
final la lista se vuelca a `mozos.dat`, de modo que el mozo número N queda en la
posición N-1 y se puede acceder directo con `fseek`.

### Planillas por día

Se obtienen las fechas distintas del archivo histórico. Para cada fecha se
vuelve a recorrer el histórico, se reemplaza el nombre por el número de mozo y
cada comanda se inserta en una lista ya ordenada por mozo (inserción estable).
Después se escribe `comandas_dd-mm-aaaa.dat`.

### Stock

El inventario está ordenado por código pero con huecos, así que se usa
**búsqueda binaria** por código. Se lee el producto, se descuenta la cantidad y
se vuelve a escribir **solo ese registro** (`fseek` a su posición + `fwrite`),
con el archivo abierto en `rb+`. No se rehace el archivo. Si el stock queda
negativo en la normalización, se deja en 0 y se avisa.

### Carga de ventas

La fecha se pide una sola vez y la planilla se abre con `"ab"` (agrega al final
y la crea si no existe). Por cada venta: se valida el mozo y su clave, se
busca el producto, se verifica que haya stock suficiente, se calcula la
comisión (`precio × cantidad × 0,10`), se descuenta el stock y se escribe la
comanda. Al terminar el día, la planilla se lee entera, se ordena por mozo
(inserción, estable) y se reescribe.

### Cierre semanal

Se pide semana, mes, año y el rango de días a cerrar. Se hace un primer
recorrido para contar las comandas de cada planilla diaria (tamaño del archivo
dividido el tamaño del registro) y se reserva memoria dinámica. Un segundo
recorrido copia los registros al arreglo, que se ordena por mozo y se escribe
en `comandas_semana_sX-mm.dat`. Si falta la planilla de algún día, se avisa y
se sigue con los demás.

### Resumen

Como la planilla semanal está ordenada por mozo, se usa **corte de control**:
se acumulan productos y comisión mientras el mozo no cambia; cuando cambia, se
imprime el anterior y se reinician los acumuladores. El último mozo no tiene
otro después que dispare el corte, por eso se imprime aparte al salir del
`while`. Si el archivo está vacío, se informa que no hay ventas.

## Casos raros contemplados

- **Mozo inexistente:** `ventas` avisa y vuelve a pedir el número.
- **Clave incorrecta:** `ventas` avisa y no deja cargar.
- **Producto sin stock:** se informa cuántas unidades quedan y no se registra
  la venta.
- **Producto inexistente:** se informa y no se registra la venta.
- **Planilla del día no creada:** `ventas` la crea al abrirla con `"ab"`.
- **Falta una planilla diaria en el cierre:** se avisa y se continúa.
- **Último mozo del resumen:** se imprime fuera del `while`.
- **Planilla semanal vacía:** `resumen` informa que no hay ventas.

## Archivos y versionado

- Los `.dat` de `datos/` son los **provistos por la cátedra**: no se modifican.
- Cualquier `.dat` que se copie a la raíz o se genere al correr **no** se
  versiona (el `.gitignore` ya los deja afuera). La cátedra corre los programas
  sobre su propio dataset.
- Todos los integrantes tienen commits propios a lo largo del trabajo.
