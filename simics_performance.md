### Aumentando el rendimiento de Simics
Simics puede resultar muy pesado de correr, en especial si no contamos con las [especificaciones recomendadas](https://www.intel.com/content/www/us/en/download/645996/30403/simics-simulator-public-release-preview.html), por lo que simics provee una herramienta que aumentará mucho el rendimiento. La tecnología se llama VMP y es compatible con la mayoría de procesadores.
Para activar VMP necesitamos tener el workspace creado y entrar en él. Necesitaremos también permisos de superusuario y ejecutar el siguiente comando
```bash
$ sudo bin/vmp-kernel-install
```
Con esto ya tendremos instalado VMP, para activarlo podemos usar el siguiente comando
```bash
$ sudo bin/vmp-kernel-load
```
VMP aumentará enormemente el rendimiento de simics. En nuestro caso, antes tomaba varios minutos solo cargar simics pero con este cambio ahora tarda menos de 10 segundos.
Es posible que vmp se desactive al reiniciar el equipo o al cambiar de workspace, sin embargo, simics mostrará en consola un comando que nos permitirá activarlo permanentemente después de ejecutar el primer comando. Este comando será diferente en cada equipo así que hay que ver la salida tras ejecutar el primer comando. En caso de no poder hacer esto último, igual vale la pena ejecutar los dos comandos anteriores, ya que el aumento de rendimiento es bastante grande, incluso, se podría agregar a algún script que se encargue de hacerlo automáticamente.