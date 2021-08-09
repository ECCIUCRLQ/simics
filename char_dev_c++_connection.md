# Conexión entre el programa de usuario y el character device.

Si el character device fue creado correctamente, siguiendo los pasos descritos en la sección correspondiente (agregar link), se debe haber creado un archivo ubicado en la ruta **/dev/** con el nombre del módulo. Aunque el character device parezca un tema muy complejo, es un simple archivo que, en lugar de tener el comportamiento por defecto al abrir, leer, escribir y cerrar, tiene un comportamiento especial definido por nosotros mediante la estructura **file_operations**, a la cual podemos asignarle punteros a nuestras propias funciones. 

Sabiendo ya que el character device es un archivo, la manera de interactuar con él es la misma en la que interactuamos con cualquier otro archivo, con la diferencia de que son nuestras funciones asignadas a la estructura file operations las que se llaman por debajo. Como programadores, esto conlleva responsabilidades dado que trabajando a nivel de kernel, un error como por ejemplo, un stack overflow, podría ocasionar un [kernel panic](https://es.wikipedia.org/wiki/Kernel_panic) y generar muchos problemas. En casos menos extremos podria generarse comportamientos que no son los esperados si no se conoce bien los valores de retorno o los parámetros que asigna el sistema operativo a nuestras funciones.

### Ejemplo en C++: read

La biblioteca estándar de C++ nos provee herramientas como ifstream, ofstream y fstream para manejar archivos fácilmente, sin embargo cualquier lenguaje de programación puede ser utilizado para comunicarse con el character device.

```cpp
#include <fstream>

int main () {
  std::ifstream chardev("/dev/Example_module");
  unsigned char buffer[4];
  chardev.read(buffer, 4);  
}
```
En el ejemplo anterior se hace un llamado a la función open de nuestro character device en la primera línea del main, luego en la tercera se hace un llamado a read, interactuando de esta forma con el character device. El método read del ifstream es diferente a la función read de nuestro character device, por lo que no deben confundirse. Puede que cada llamado a read de ifstream no corresponda a un llamado a read del character device, pero esto lo abordaremos más adelante.

### Función open del character device
Por lo general no hay que tomar muchas consideraciones al abrir el archivo pero se puede utilizar para inicializar ciertas variables o para hacer que la apertura falle en caso de darse alguna condición o error. La función retorna un entero, que en caso de no ser 0, nuestro ifstream lo considerará un error y es posible que falle abrir el archivo, por lo que es responsabilidad del programador, asegurarse de retornar el valor correcto y asegurarse de que el archivo se haya abierto correctamente usando el método **is_open** en el caso de los streams de C++. Los parámetros de esta función son dos punteros a estructuras que suelen no ser de mucha utilidad. Son datos que usa el sistema operativo para el manejo de nuestro archivo y los podemos acceder en caso de ser necesario.
```cpp
int open(struct inode *inode, struct file *file);
```

### Función read del character device
El read es un método que puede generar problemas o comportamiento no deseado si no entendemos bien que valores se deben retornar y cuales son los parámetros. Además, hay que tener cuidado de no sobrecargar la pila con vectores muy grandes mientras preparamos los datos que queremos copiar al usuario, ya que un stack overflow en este punto podría ocasionar un Kernel Panic.
```cpp
ssize_t read(struct file *file, char __user *buf, size_t len, loff_t *offset);
```
El primer parámetro es nuevamente información del archivo que no será de mucha utilidad. El segundo parámetro es el buffer del usuario al cual deberemos escribir para hacerle llegar la información que queramos. Hay que tener ciertas consideraciones con este buffer. Primero, es importante verificar el tamaño del buffer y asegurarnos de que lo que queramos escribir no sea de mayor tamaño. El tercer parámetro nos indica precisamente el tamaño del buffer por lo que deberíamos verificarlo antes de escribir cualquier cosa. Escribir directamente a este buffer es una vulnerabilidad, ya que el kernel puede escribir libremente a cualquier región de memoria sin realizar ninguna verificación. Por esto, linux nos provee la función **copy_to_user** que nos permite hacer lo mismo de manera más segura y portable.
```cpp
copy_to_user(void __user *to, const void *from, unsigned long n);
```
De primer parámetro recibe el arreglo al cual queremos copiar, buf en este caso, el arreglo desde el cual queremos copiar de segundo parámetro, y la cantidad de bytes a copiar como tercer parámetro. Si la cantidad de bytes a copiar *n* es mayor que el tamaño del buffer *len* tendremos un buffer overflow y fallará la copia. Igualmente se podrían generar errores si usamos punteros en lugar de arreglos, como lo indican [aquí](https://stackoverflow.com/a/68195370).
El cuarto parámetro de read *offset* es un contador que podemos utilizar para saber cuantos bytes hemos copiado. Es un valor entero que podemos usar libremente al dereferenciar el puntero e incluso podemos retornarlo para indicar cuantos bytes fueron leídos, ahorrándonos el tener que declarar otra variable para este fin.
Los ifstream en C++ funcionan con un buffer interno, que de no estar vacío, no hará lecturas al archivo hasta vaciarlo. El valor de retorno le indica al ifstream cuantos bytes se leyeron. Si nuestra función read del character device retorna 20, entonces el ifstream considerará cargados 20 bytes en su buffer interno, incluso si parte de estos 20 no fueron modificados y tienen basura sin inicializar. Con 20 bytes leídos, le permitiría al ifstream ejecutar la tercera linea del main del [ejemplo anterior](#ejemplo-en-c-read) 5 veces con un solo llamado a read del character device, ya que lee de 4 en 4 los 20 bytes que se cargaron. Por el contrario si nuestro read retorna 4 entonces cada llamado al read del ifstream correspondería a un llamado al read del character device.
También es posible utilizar el operador **>>** para leer del ifstream aunque es para salida con formato por lo que podría ignorar espacios en blanco o saltos de línea.

### Ejemplo en C++: write
```cpp
#include <fstream>

int main () {
  std::ofstream chardev("/dev/Example_module");
  unsigned char byte = 45
  unsigned char buffer[] = {'H','O','L','A'};
  chardev.write(buffer, 4);
  chardev << byte;
  chardev.flush();  
}
```
En este ejemplo, también hay un llamado a open en la primera linea del main, y en la cuarta y quinta se hacen escrituras de dos formas diferentes, con el operador **<<** y con el método write de ofstream. Es recomendable usar write si lo que queremos escribir no tiene un formato específico. Los ofstream en C++ también funcionan con un buffer interno, que al llenarse, hará un llamado a *flush* automáticamente. Esto es así por cuestiones de eficiencia, sin embargo podemos llamar a flush manualmente como en la última línea del ejemplo. El método write de nuestro character device no será llamado hasta que no se realice un flush, por lo que en el ejemplo, tanto la escritura de *byte* como la escritura de *buffer* corresponden a un único llamado al write del character device, dado a que se enviaron en un mismo *flush*.

### Función write del character device
La función write, tiene los mismos parámetros que read, sin embargo, el buffer que nos proveen como segundo parámetro ya tiene bytes escritos desde el programa de usuario, y el tercer parámetro es nuevamente el tamaño de este buffer. Lo que debemos hacer es copiar los bytes de este buffer a un vector propio mediante la función **copy_from_user** que funciona de manera similar a **copy_to_user**, ya que podría ser riesgoso usar el buffer directamente. Este vector propio no debe ser demasiado grande por que el tamaño de la pila es limitado y pueden generarse un stack overflow, además, se deben tener las [mismas consideraciones](https://stackoverflow.com/a/68195370) que con **copy_to_user**. El valor de retorno debe ser la cantidad de bytes que se escribieron y en caso de retornar un valor diferente, se afecta el comportamiento del buffer de nuestro ofstream. El parámetro *offset* puede usarse nuevamente para llevar un conteo de cuantos bytes se han escrito.

### Función release del character device
Esta función se invoca solo cuando nuestro fstream se destruye o se llama manualmente a **close()**. Nuevamente no hay que tener demasiadas consideraciones más que reiniciar variables en caso de necesitarse o liberar memoria en caso de haber utilizado memoria dinámica.

### Comunicación entre el character device y el dispositivo PCI
Si nuestro character device se encuentra en el mismo archivo que nuestro driver pci, se puede llamar perfectamente las funciones del driver en el cuerpo de las funciones del character device. Por lo que dentro de nuestra función read o write se puede hacer directamente un *ioread* o *iowrite* a los bars del dispositivo PCI, haciendo muy fácil la comunicación entre el programa de usuario y el dispositivo.
