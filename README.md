# Proyecto de Monitorización de Luz

## Descripción

Este proyecto captura y monitorea la intensidad lumínica utilizando una combinación de sensores y componentes electrónicos. Se utiliza una ESP32 para procesar los datos de un sensor de luz VEML 7700 y un sensor de tiempo real (RTC) HW 084. Los datos se almacenan en una tarjeta micro SD en caso de que no haya conexión a internet, y el circuito es alimentado por un módulo de carga J5019. Los datos de luz se envían a través de Mosquitto MQTT a Node-RED, donde se almacenan en una base de datos PostgreSQL. Además, los datos se visualizan en tiempo real y se pueden generar gráficas personalizadas seleccionando rangos de fechas específicos.

## Objetivos del Proyecto

- **Medición Precisa de Luz**: Capturar la intensidad lumínica con el sensor VEML 7700.
- **Tiempos Precisos**: Registrar datos con marcas de tiempo precisas usando el sensor RTC HW 084.
- **Almacenamiento Seguro**: Guardar datos en una tarjeta micro SD cuando no haya conexión a internet.
- **Gestión de Energía**: Utilizar un módulo de carga J5019 para alimentar de manera eficiente el circuito.
- **Transmisión de Datos**: Enviar los datos recogidos a través de Mosquitto MQTT hacia Node-RED.
- **Almacenamiento Centralizado**: Almacenar los datos en una base de datos PostgreSQL.
- **Visualización de Datos**: Proporcionar gráficos en tiempo real y la capacidad de seleccionar y visualizar datos en rangos de fechas específicos.

## Componentes Utilizados

- **ESP32**: Microcontrolador principal para el procesamiento de datos.
- **Sensor de Luz VEML 7700**: Captura la intensidad lumínica.
- **RTC HW 084**: Proporciona marcas de tiempo precisas.
- **Micro SD Card Reader**: Almacenamiento de datos offline.
- **Módulo de Carga J5019**: Gestiona la alimentación del circuito.
- **Mosquitto MQTT**: Protocolo de comunicación para la transmisión de datos.
- **Node-RED**: Plataforma para la integración y visualización de datos.
- **PostgreSQL**: Base de datos para almacenar los datos de luz y tiempo.

## Instalación y Configuración

1. **Hardware**:
   - Conectar el sensor de luz VEML 7700, el RTC HW 084, el lector de tarjetas micro SD y el módulo de carga J5019 a la ESP32 según las especificaciones del fabricante.

2. **Software**:
   - Clonar este repositorio:
     ```bash
     git clone https://github.com/tu_usuario/proyecto-monitorizacion-luz.git
     ```
   - Configurar el entorno de desarrollo para la ESP32.
   - Instalar Mosquitto MQTT en el servidor.
   - Configurar Node-RED para recibir y procesar los datos.
   - Configurar PostgreSQL y crear la base de datos necesaria para almacenar los datos.

3. **Ejecución**:
   - Cargar el código en la ESP32 y encender el dispositivo.
   - Verificar que los datos se estén enviando correctamente a Node-RED y almacenando en PostgreSQL.
   - Acceder a Node-RED para visualizar los datos en tiempo real y utilizar las herramientas de selección de rangos de fechas para generar gráficos personalizados.

## Uso del Proyecto

- **Visualización en Tiempo Real**: Monitorea la intensidad lumínica en tiempo real a través de la interfaz de Node-RED.
- **Selección de Rangos de Fechas**: Utiliza la funcionalidad de selección de fechas en Node-RED para generar gráficos personalizados basados en los datos almacenados en PostgreSQL.

