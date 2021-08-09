# Conceptos de sistemas operativos relacionados al driver

## ¿Qué es un sistema operativo?

> Un sistema operativo, SO u también OS (Operating System), es el software encargado de gestionar los recursos hardware y proveer de servicios al resto de programas que se ejecuten sobre él, siendo el sistema quien se ejecuta siempre en modo privilegiado con respecto al resto.(Adeva, 2021, párr.18)

En palabras más simples, un sistema operativo es una interfaz o intermediario entre el hardware y el software de la máquina.

![Esquema de cómo se ve un sistema operativo](https://www.adslzone.net/app/uploads-adslzone.net/2020/03/funconSO-715x492.jpg)


## Características o responsabilidades de un sistema operativo:

* Manejo de memoria.
* Manejo del CPU.
* Manejo de la interacción con los dispositivos de entrada y salida.
* Configuración de registros.
* Manejo de archivos.
* Seguridad.
* Rendimiento.
* Logs.
* Detección de errores.
* Coordinación.

## El sistema operativo a usar: Linux.

Linux es una distribución de un sistema operativo de tipo "open source". Este tiene la característica de que dentro de él **todo es un archivo**.

## Espacio kernel y espacio usuario:

En Linux, el sistema se encuentra dividido en dos espacios principales, el espacio de kernel y el de usuario. 
Para este proyecto, cuando se quiera trabaja el driver, la parte en la que se debe de trabajar debe ser en el espacio de kernel, nunca en el de de usuario, esto debido a que el sistema operativo es el único que puede tener acceso al hardware, por lo que el código necesita estar en el kernel.

Por esta razón es importante conocer los siguientes conceptos:

**Kernel:** 

> Se define como la parte que se ejecuta en modo privilegiado o modo núcleo y que es el principal responsable a la hora de arrancar el propio sistema, proporcionar un control de nivel básico sobre todos los componentes hardware del ordenador y cuyas funciones principales son leer y escribir datos en memoria, procesar órdenes, interpretar datos, decidir qué programar podrá hacer uso de un determinado recurso y durante cuánto tiempo, etc. (Adeva, 2021, párr.21)

**Interfaz de usuario:**

> "Se refiere a todo el código en un sistema operativo que vive fuera del kernel."(McCarty. 2015, párr.9)
Puede ser gráfica o no. Es la parte que le permite al usuario interactuar con la máquina.


La comunicación entre estos dos espacios ocurre mediante "system calls" o llamados al sistema. Como se puede observar  en la sisguiente imagen:

![Esquema de comunicación entre el espacio de kernel y el espacio de usuario](https://www.redhat.com/cms/managed-files/styles/wysiwyg_full_width/s3/2015/07/user-space-vs-kernel-space-simple-user-space.png?itok=7PGYkTdC)


## Driver:

Un driver es una pieza intermediaria que permite comunicar el hardware con el espacio de ususario, por lo que actúa como un puente entre el espacio de kernel y el de usuario. Este es una parte del Kernel.
El driver tiene dos piezas importantes el módulo de "character device", que permite que la interación con el usuario sea sencilla, y  el módulo de "PCI", que es una serie  de reglas  para poder comunicarse con el "PCI", que es un estándar.

* ### Módulos del kernel:

..* Tipos:
...* Character device
...* PCI/ PCI-express
...* Compiladores: cualquier código de kernel se compila de una forma distinta a la acostumbrada, para esto ver la entrada: [Makefile para Kernel](https://github.com/ECCIUCRLQ/simics/blob/main/Makefile.png)
...* Ejecución: Se debe instanciar e instalar en el kernel para poder ejecutarlo.
...* Depuración: Se puede utilizar un "printk", que es un "print" para driver, con el fin de visualizar errores. 


## Referencias:

Adeva, R. (2021). Historia y todo lo que debes saber sobre un sistema operativo. ADSLZone. Retrieved 8 August 2021, from https://www.adslzone.net/reportajes/software/que-es-sistema-operativo/.
McCarty, S. (2015). Architecting Containers Part 1: Why Understanding User Space vs. Kernel Space Matters. Red Hat. Retrieved 8 August 2021, from https://www.redhat.com/en/blog/architecting-containers-part-1-why-understanding-user-space-vs-kernel-space-matters.
