# omnetpp-basic
En este repositorio se ha subido la implementación del escenario básico (una fuente, un emisor y un receptor) desarrollado en OMNeT++. A continuación se explica en mayor detalle el procedimiento seguido, así como una explicación de los métodos implementados, problemáticas encontradas o algunos comentarios acerca del mismo.

## Estructura del proyecto
Antes de comenzar a explicar el funcionamiento de cada uno de los módulos, se ha creído conveniente explicar brevemente la estructura de ficheros del proyecto de modo que sea más sencilla la comprensión posterior:

* `network.ned`: en este fichero se definen los módulos que se van a utilizar, así como sus enlaces, conexiones y otros parámetros básicos. Además, aquí es donde se definen las redes o *networks* (agrupaciones de módulos que están conectados y que interaccionan entre sí durante la simulación).

* `omnetpp.ini`: en este fichero se permiten inicializar/modificar ciertos parámetros de los módulos del network.ned. De esta forma, en el .ned se definen los parámetros por defecto de cada módulo y en el .ini se inicializan con los valores que correspondan (para cada instancia). 

* `myPacket.msg`: en este fichero se define la estructura de paquetes que se va utilizar en la simulación. A partir de este fichero, el compilador genera los ficheros `myPacket_m.cc` y `myPacket_m.h`.

* `Source.cc`: este fichero define el comportamiento del módulo de fuente o generador de paquetes. Este módulo se encarga de generar paquetes con una tasa exponencial y con la estructura definida en `myPacket.msg`.

* `Sender.cc`: este fichero define el comportamiento del módulo de emisor. Éste es el encargado de gestionar el envío de paquetes a otros nodos de la red.

* `Receiver.cc`: este fichero define el comportamiento del módulo de receptor. Éste es el encargado de gestionar la recepción de paquetes provenientes de otros módulos, comprobar si tienen errores y generar los paquetes de *acknoledge* ack/nack correspondientes.

## Topología de red
El esquema de red que se ha definido es el siguiente:

![Esquema de red](/img/esquema.png)

En este esquema se pueden observar los tres módulos simples definidos: sender, receiver y source. Además, para simular el envío de paquetes a través de enlaces "reales" (con probabilidad de error, retardo, etc.) se ha definido el canal *Ethernet100*. Este canal se ha utilizado para unir el módulo emisor y el de receptor.

## Funcionamiento
En este apartado se explican en mayor detalle los módulos de fuente, emisor y receptor de paquetes.
### Source.cc
Este módulo cuenta con las siguientes funciones:
* `initialize()`: en esta función se inicializa la tasa de generación de paquetes y el número de secuencia. Además, en esta función se programa un evento propio o *selfmessage* que se utilizará para crear nuevos paquetes al de un tiempo determinado.

* `createPck()`: esta función se encarga de generar un nuevo paquete que sigue la estructura de `myPacket.msg`.

* `handleMessage(cMessage *msg)`: esta función se lanza cada vez que el módulo recibe un nuevo evento. Con cada evento, se genera un nuevo paquete con `createPck()`, se envía el paquete y se programa un nuevo evento para dentro de un tiempo determinado (tasa exponencial).

### Receiver.cc
Este módulo cuenta con las siguientes funciones:
* `initialize()`: no hace nada.

* `createPck()`: esta función se encarga de generar un nuevo paquete de *acknoledge* ack o nack.

* `handleMessage(cMessage *msg)`: esta función se lanza cada vez que el módulo recibe un nuevo evento/paquete. Cada vez que llega un paquete, se comprueba si tiene error: si tiene, se crea un nack y si no tiene, un ack (vía `createPck()`). Finalmente, se envía dicho paquete de confirmación.

### Sender.cc
Este módulo cuenta con las siguientes funciones:
* `initialize()`: esta función se encarga de inicializar la cola y la máquina de estados. Para simular el comportamiento de un nodo que implementa una política FIFO y Stop & Wait para el envío de paquetes, es necesario **definir una maquina de estados que permita controlar cuando se puede enviar un nuevo paquete por el enlace y una cola para almacenar los paquetes que lleguen al emisor mientras el enlace por el que se quieran enviar se encuentre ocupado.**

* `sendCopyOf(myPacket *pck, int index)`: esta función se encarga de gestionar el envío de los paquetes. En primer lugar duplica el paquete, envía el paquete original y la copia la almacena en la cola **en primera posición, de modo que en caso de que llegue una retransmisión se pueda extraer facilmente de la cola.** Después, se cambia la maquina de estados a *STATE_BUSY* para que no se pueda volver a enviar paquetes a través de ese enlace hasta que no llegue la confirmación y, finalmente, se programa un evento de *timeout* para que en caso de que se pierda el paquete se vuelva a retransmitir.

* `handleMessage(cMessage *msg)`: esta función es la encargada de gestionar la forma de operar del emisor. En concreto, ésta sigue el siguiente procedimiento:

  1. En primer lugar se comprueba si el evento recibido es de *timeout*. Si lo es, extrae el paquete de la cola y lo retransmite (vía `sendCopyOf()`).

  2. En segundo lugar, comprueba si es un paquete que ha llegado desde la fuente. Si es así, comprueba el estado actual de la máquina de estados y, en función de si el enlace se encuentra ocupado (STATE_BUSY) o libre (STATE_IDLE), lo inserta en la cola o lo envía mediante la función `sendCopyOf()`.

  3. Si no es ninguno de los casos anteriores, quiere decir que el evento es un paquete que proviene del receptor. Entonces, se comprueba que tipo de paquete es: un ack (TYPE_ACK) o nack (TYPE_NACK):

    * **TYPE_ACK:** este paquete indica que el paquete enviado al receptor ha llegado correctamente. Por lo tanto, se puede eliminar la copia almacenada en la cola y enviar el siguiente en el caso de que la cola no estuviese vacía.
  
    * **TYPE_NACK:** este paquete indica que el receptor ha recibido el paquete que se ha enviado con errores. Por lo tanto, el emisor tiene que volver a enviar el paquete (la copia que se habia almacenado en la cola) vía `sendCopyOf()`.
