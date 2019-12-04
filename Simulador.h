//////////////////////////////////////////////////////////////
//
// Fichero: Simulador.h
//
// Grupo trabajo: 4
//
// Autores:
//      LUCIA REINA LOPEZ 
//      ALVARO MENACHO RODRIGUEZ 
//      AURORA MARIA SALVADOR GUTIERREZ
//      Luis Rolando Cahuana Leon
//
// Contenido: Definicion de constantes, estructuras y funciones
//     usadas en las simulaciones
//
//////////////////////////////////////////////////////////////

#ifndef SIMULADOR_H
#define SIMULADOR_H

#include <iomanip>

#include "ns3/object.h"
#include "ns3/global-value.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/error-model.h"
#include "ns3/random-variable-stream.h"
#include "ns3/gnuplot.h"
#include "ns3/traffic-control-layer.h"
#include "ns3/traffic-control-helper.h"
#include "bridge-helper.h"

#include "Observador.h"

/********************************************/
/* DEFINES                                  */
/********************************************/

// Topologia

// PPP
#define RETARDO_PROP_PPP    "2ms"
#define CAPACIDAD_PPP       "150kbps"

// Trafico
#define TAM_PAQUETE 512 //En Octetos
#define TASA_ENVIO  "3Mbps"
#define T_ON        "250ms"
#define T_OFF       "125ms"
#define T_START     "10s"
#define T_STOP      "30s"
#define TAM_COLA    5

// Red
#define SUBRED_PPP  "10.20.20.0"
// Mascara es igual en ambas subredes
#define MASCARA	    "255.255.255.0"
#define PORT        9

// Simulacion
#define NUM_SIM_POR_PUNTO 10
#define NUM_CURVAS        5
// Valor de la tStudent para un IC del 95% con 10 simulaciones
#define TSTUDENT          2.2622

// Datos
#define TODAS_TRAZAS_PCAP false

// Indice en la lista de resultados
#define RES_RETARDO_MEDIO  0
#define RES_PORUNO_CORREC  1

//Parámetros para las las funciones de distribución de los videos.
//Vídeo 1
#define MU1=0.4026
#define SIGMA1=0.0352
#define SHAPE1=10.2063
#define SCALE1=57480.9
#define MAXBYTES1=10989173
//Vídeo 2
#define MU2=
#define SIGMA2=0.0352
#define SHAPE2=10.2063
#define SCALE2=57480.9
#define MAXBYTES2=10989173

using namespace ns3;

/********************************************/
/* TIPOS DE DATOS                           */
/********************************************/

/**
 * Contiene los parametros de configuracion de la topologia
 */
typedef struct {
    // PPP
    Time *retardoPropPPP;
    DataRate *capacidadPPP;
} ConfigTopologia;

/**
 * Contiene los parametros de configuracion del trafico
 */
typedef struct {
    uint32_t tamPaquete;
    DataRate *tasaEnvio;
    Time *tOn;
    Time *tOff;
    Time *tstart;
    Time *tstop;
    
} ConfigTrafico;

/**
 * Contiene los parametros de configuracion de las salidas
 */
typedef struct {
    // Muestra las trazas pcap
    bool todasTrazas;
    
} ConfigDatos;

/********************************************/
/* DECLARACION DE FUNCIONES                 */
/********************************************/

/**
 * Convierte un double en string con dos posiciones detras de la coma
 * Fuente del codigo:
 * https://thispointer.com/c-convert-double-to-string-and-manage-precision-scientific-notation/
 *
 * @param num Numero a covertir
 *
 * @returns num convertido a string
 */
std::string convString(double num);

/**
 * Inicia los parametros de configuracion a los valores por defecto
 *
 * @param[out] confTopologia Configuracion de la topologia por defecto
 * @param[out] confTrafico Configuracion del trafico por defecto
 * @param[out] confDatos Configuracion de la representacion de datos
 * 	por defecto
 */
void initParams(ConfigTopologia *confTopologia, ConfigTrafico *confTrafico,
    ConfigDatos *confDatos);

/**
 *   Lee los parametros introducidos en la linea de comandos y los
 *   carga en las variables de configuracion pasadas como parametro
 *
 *   @param argc num de argumetos introducidos por linea de comandos
 *   @param argv argumentos introducidos por linea de comandos
 *   @param[out] confTopologia contendra la configuracion relativa a
 *       la topologia de la red
 *   @param[out] confTrafico contendra la configuracion relativa al
 *       trafico generado por las aplicaciones clientes
 *   @param[out] confDatos contendra la configuracion relativa a
 *       la presentacion de los datos
 */
void leeParamLineaCommandos(int argc, char *argv[],
    ConfigTopologia *confTopologia, ConfigTrafico *confTrafico,
    ConfigDatos *confDatos);

#endif
