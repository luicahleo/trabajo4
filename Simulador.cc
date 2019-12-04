//////////////////////////////////////////////////////////////
//
// Fichero: Simulador.cc
//
// Grupo trabajo: 4
//
// Autores:
//      LUCIA REINA LOPEZ 
//      ALVARO MENACHO RODRIGUEZ 
//      AURORA MARIA SALVADOR GUTIERREZ
//      Luis Rolando Cahuana Leon
//
// Contenido: Contiene el metodo main que configura el 
//     escenario, realiza las simulaciones, y generamos
//     los resultados
//
//////////////////////////////////////////////////////////////

#include "Simulador.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Simulador");

/********************************************/
/* MAIN                                     */
/********************************************/

int main(int argc, char *argv[]) {
    NS_LOG_FUNCTION_NOARGS();
    GlobalValue::Bind("ChecksumEnabled", BooleanValue(true));
    Time::SetResolution(Time::NS);

    // Parametros de la Simulacion
    ConfigTopologia confTopologia;
    ConfigTrafico confTrafico;
    ConfigDatos confDatos;
    initParams(&confTopologia, &confTrafico, &confDatos);
    leeParamLineaCommandos(argc, argv, &confTopologia, &confTrafico,
        &confDatos);

    return 0;
}

/********************************************/
/* DEFINICION DE FUNCIONES                  */
/********************************************/

std::string convString(double num) {
    // Create an output string stream
    std::ostringstream streamObj;
    // Set Fixed -Point Notation
    streamObj << std::fixed;
    // Set precision to 2 digits
    streamObj << std::setprecision(2);
    //Add double to stream
    streamObj << num;
    // Get string from output string stream
    return streamObj.str();
}

void initParams(ConfigTopologia *confTopologia, ConfigTrafico *confTrafico,
        ConfigDatos *confDatos) {
    // Topologia
    // PPP
    confTopologia->retardoPropPPP = new Time(RETARDO_PROP_PPP);
    confTopologia->capacidadPPP = new DataRate(CAPACIDAD_PPP);

    // Trafico
    confTrafico->tamPaquete = TAM_PAQUETE;
    confTrafico->tOn = new Time(T_ON);
    confTrafico->tstart = new Time(T_START);
    confTrafico->tstop = new Time(T_STOP);

    // Datos
    confDatos->todasTrazas = TODAS_TRAZAS_PCAP;
}

void leeParamLineaCommandos(int argc, char *argv[],
        ConfigTopologia *confTopologia, ConfigTrafico *confTrafico,
        ConfigDatos *confDatos) {
    NS_LOG_FUNCTION_NOARGS();
    CommandLine cmd;
    // Topologia
    // PPP
    cmd.AddValue("retardoPropPPP", "Retardo de propagacion en el enlace ppp",
        *confTopologia->retardoPropPPP);
    cmd.AddValue("capacidadPaP", "Capacidad del enlace PaP",
        *confTopologia->capacidadPPP);
    // Trafico
    cmd.AddValue("tamPqt", "Tamaño de la sdu de aplicacion en octetos",
        confTrafico->tamPaquete);
    cmd.AddValue("ton", "Valor inicial de tiempo medio de permanencia en el "
        + std::string("estado ON"), *confTrafico->tOn);
    cmd.AddValue("tstart", "Instante de simulacion de inicio de las "
        + std::string("aplicaciones clientes"), *confTrafico->tstart);
    cmd.AddValue("tStop", "Tiempo de duracion de la actividad de las fuentes "
        + std::string("clientes"), *confTrafico->tstop);
    // Datos
    cmd.AddValue("todasTrazas", "Guardar las trazas pcap de todos los nodos ", 
        confDatos->todasTrazas);
    
    cmd.Parse(argc, argv);

    // Calculo de valores que no introducen por linea de comandos
    // toff es la mitad de ton
    double tiempoOff = confTrafico->tOn->GetSeconds() / 2;
    std::string tOffStr = std::to_string(tiempoOff) + "s";
    NS_LOG_INFO("Tiempo de off de los clientes = " << tOffStr );
    confTrafico->tOff = new Time(tOffStr);
    
   
}

void escenario(){
  // Creamos los contenedores para los nodos
  // Nodo del switch
  NodeContainer nodoSwitch;
  nodoSwitch.Create(1);

  // Nodo Servidor 
  NodeContainer nodoServidor;
  nodoServidor.Create(1);

  // Nodos clientes
  NodeContainer nodosClientes;
  nodosClientes.Create(1);

  // Nodos finales (clientes y servidor) para instalar la pila
  NodeContainer nodosFinales;
  nodosFinales.Add(nodoServidor);
  nodosFinales.Add(nodosClientes);
  
  // Además, nos hará falta un contenedor de nodos por cada enlace punto a punto
  NodeContainer enlaceSwitchServidor;
  enlaceSwitchServidor.Add(nodoSwitch);
  enlaceSwitchServidor.Add(nodoServidor);
  
  NodeContainer enlaceSwitchCliente;
  enlaceSwitchCliente.Add(nodoSwitch);
  enlaceSwitchCliente.Add(nodosClientes);

  // Helper para PointToPoint
  // Configuramos las características de los enlaces PAP
  PointToPointHelper puntoAPunto;
  puntoAPunto.SetChannelAttribute("Delay", TimeValue(confTopologia->retardoPropPPP));
  puntoAPunto.SetDeviceSttribute("DataRate", DataRateValue(confTopologia->capacidadPPP));
  
  // Instalamos PPP en los dispositivos de la topología
  NetDeviceContainer dispSwitchSer;
  dispSwitchSer= puntoAPunto.Install(enlaceSwitchServidor);
  NetDeviceContainer dispSwitchCli;
  dispSwitchCli= puntoAPunto.Install(enlaceSwitchCliente);
  
  // Este container almacenará todos los dispositivos del escenario que tengan pila IP
  NetDeviceContainer switchPorts;
  // Recuperamos del primer container el servidor y del segundo todos los clientes
  // Empezamos en 1 ya que el switch esta el primero en ese container
  switchPorts.Add(dispSwitchSer->Get(1));
  for (uint32_t port=1;port<=dispSwitchCli->GetN()-1;port++){
    switchPorts.Add(dispSwitchCli->Get(port));
  }  
  NetDeviceContainer switchDevices;
  BridgeHelper bridgeHelper;
  switchDevices= bridgeHelper.Install(nodoSwitch,switchPorts);

  // Instalamos la pila UDP/IP solo en los nodos finales, no en los puentes transparentes
  InternetStackHelper stack;
  stack.SetIpv6StackInstall(false);
  stack.Install(nodosFinales);

  // Asignamos las direcciones a los nodos
  Ipv4AddressHelper address (SUBRED_PPP, MASCARA);
  /*  ***** Quizás no haga falta crear ifppp y debamos poner 
  address.Assign(switchDevices) únicamente  ***** */
  Ipv4InterfaceContainer ifppp = address.Assign(switchDevices);

  // Calculamos las rutas
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Configuramos las aplicaciones de los dispositivos
  // El servidor será una fuente OnOff
  OnOffHelper fuente ("ns3:UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), PORT));
  //Configuramos los parametros de la fuente
  
  //Instalamos la fuente
  ApplicationContainer fuenteApp = fuente.Install(nodoServidor);
  //Sumidero en los clientes
  PacketSinkHelper sumidero ("ns3::UdpSocketFactory", Address(InetSocketAddress(Ipv4Address::GetAny(), PORT)));
  ApplicationContainer sumideroApp= sumidero.Install(nodosClientes);
  
  // Configuramos las variables aleatorias para el tráfico
  // TO-DO

  // Establecemos los atributos de la fuente
  /* ***** ¿cómo enviamos a todos los dispositivos? ¿un bucle
  machacaría el atributo remote?
  La dirección de difusión no, porque entonces no podríamos ver cuántos clientes
  soportan la recepción del vídeo. (Lo ha dicho Germán en clase)
  Una dirección multicast funcionaría, pero entonces no sería VoD
  Dijo que probablemente haría falta un bucle: probarlo
  ***** */

  
  fuente.SetAttribute("Remote", );
  fuente.SetAttribute("PacketSize", );

}
