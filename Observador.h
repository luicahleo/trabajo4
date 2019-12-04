//////////////////////////////////////////////////////////////
//
// Fichero: Observador.h
//
// Pareja: 19
//
// Autores:
//     Miguel Angel Gonzalez-Alorda Cantero
//     Fernando Martín Ramos-Catalina
//     Luis Rolando Cahuana Leon
//
// Contenido: Definicion de la clase Observador, que mide
//     los retrasos y las perdidas de los paquetes enviados
//     por las aplicaciones
//
//////////////////////////////////////////////////////////////

#ifndef OBSERVADOR_H
#define OBSERVADOR_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/callback.h"
#include "ns3/onoff-application.h"
#include "ns3/packet-sink.h"
#include "ns3/average.h"

using namespace ns3;

class Observador : public Object
{
    public:
        Observador(Ptr<OnOffApplication> cliente_ptr, Ptr<PacketSink> servidor_ptr);

        void ProgramaTrazas();

        // Getters
        double getRetardoAverage();
        uint64_t getNumPacketsTx();
        uint64_t getNumPacketsRx();
    private:
        // Callbacks
        void clienteTx(Ptr<const Packet> pqt);
        void servidorRx(Ptr<const Packet> pqt, const Address &address);
        
        Ptr<OnOffApplication> m_cliente_ptr;
        Ptr<PacketSink> m_servidor_ptr;
        // Mapa con clave el uid de los paquetes Tx por el cliente y por valor 
        // el instante de simulacion en nanosegundos en que se creo
        std::map<uint64_t, uint64_t> m_packetsTx;
        uint64_t m_numpacketsTx;
        uint64_t m_numpacketsRx;
        // Estadisticos del retardo total desde que se crea el pkt en el cliente 
        // hasta que lo recibe el servidor, almacenado en nanosegundos
        Average<uint64_t> m_retardoAcum;
};
#endif

