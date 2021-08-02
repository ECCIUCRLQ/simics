# Cómo instalar Simics
La instalación de este programa se debe hacer en una máquina Linux, puede seguir los pasos que se detallan a continuación para realizarlo, tome en cuenta que estos comandos son para Ubuntu que es el Linux que se usó para este proyecto, por lo que para otras versiones de Linux puede ser distinto:
1. Descargar ambos archivos disponibles en el siguiente enlace: https://downloadcenter.intel.com/download/30403/Intel-Simics-Public-Release-Preview
2. Una vez descargados ambos archivos por medio de la terminal vamos a la carpeta donde están descargados y ejecutamos los siguientes comandos:
  ```console
  foo@bar:~$ ./ispm packages --install-bundle direccion/simics-6-packages-2021-16-linux64.ispm direccion/donde/quiere/instalarlo
  ```
3. Con el paso anterior ya deberia estar instalado Simics, ahora queda instalar los paquetes de Simics. Abra la aplicación para instalación gráfica de los paquetes, aquí es donde usaremos el archivo compromido que fue descargado:
  ```console
  foo@bar:~$ ./ispm-gui
  ```
4. Una vez abierta la aplicación gráfica simplemente seleccione donde le da la opción de escoger los paquetes a instalar el archivo .tar que había descargado. Y listo, una vez hecho eso lo que resta es crear el workspace.

Autor: Emmanuel D. Solis.