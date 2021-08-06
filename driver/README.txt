Algunas de las referencias utiles para construir el driver fueron las siguientes:

En esta seccion encontraremos la forma de confeccionar un esqueleto para nuestro driver, es 
decir, no tendra una funcionalidad especifica pero tendra lo minimo necesario para considerarse 
un driver, haciendo que este sea la base para iniciar la confeccion de un driver especializado
para las funcionalidades de nuestro device. 

Sin capacidades especificas y bastante util para comenzar

    Writing a PCI device driver for Linux
    by Oleg Kutkov
    January 7, 2021
    https://olegkutkov.me/2021/01/07/writing-a-pci-device-driver-for-linux/


En esta seccion encontraremos las primeras formas de darle vida a nuestro driver, con el CHAR DEVICE
(Character Device) de nuestro driver, definiremos las primeras funcionalidades para interactuar entre el 
device y el driver (el dispositivo y el controlador)

Esta seccion es una buena base para explorar como construir o programar funcionalidades sumamente basicas 
para el driver, sin embargo, es importande adaptar las mismas para el device que estamos desarrollando, por 
lo cual se recomienda experimentar en la simulacion en simics, entender su funcionamiento o solicitar ayuda
para entenderlo, y seguidamente proceder a modificarlo segun nuestras necesidades. 

    Simple Linux character device driver
    by Oleg Kutkov
    March 14, 2018
    https://olegkutkov.me/2018/03/14/simple-linux-character-device-driver/

Adicional a esto, cabe mencionar el ejemplo dentro de los archivos de entrenamiento de simics, si bien este
ejemplo dentro de estas carpetas es bastante mas complicado de entender, puede ser recomendable entenderlo 
para intentar dar el siguiente paso en el desarrollo del driver, aunque no es estrictamente necesario o 
indispensable. El mismo se encuentra en el directorio o carpeta 
./simics-training-6.0.pre13/targets/simics-user-training/target-source/simics-training-pcie-driver como 
simics-training-pcie-driver.c dentro de los packs instalados de simics (diferente de nuestro work space)

Algunas recomendaciones para los primeros abordajes, probablemente nos preguntemos porque la IDE que estamos 
usando marca errores, como en la inclusion de las librerias, o como ciertas secciones del codigo que esperamos
se resalten como variables, metodos, punteros o demas ayudas visuales no estan funcionando, bueno esto se debe 
porque estamos programando para el entorno virtual de simics y no para el entorno en donde se encuentra instalada 
la IDE. Por eso, por ejemplo, no reconece las librerias que tratamos de incluir, estas van a estar en la simulacion 
asi que no debemos preocuparnos por eso, sigue adelante, sin embargo, ten paciencia cuando intentes debuggear tu driver
ya que los errores los veras hasta estar en la simulacion, como por ejemplo, metodos que no puedes utilizar que estas
muy habituade a usar como ciertos prints u otros. Pero recuerda, estas programando fuera del entorno donde vas a correr
el programa. 

Tu primer driver podras encontrarlo aca con el nombre de "my_first_driver.c" muchos exitos y disfruta