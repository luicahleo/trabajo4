//////////////////////////////////////////////////////////////
//
// Fichero: Observador.cc
//
// Pareja: 19
//
// Autores:
//     Miguel Angel Gonzalez-Alorda Cantero
//     Fernando Martín Ramos-Catalina
//     Luis Rolando Cahuana Leon
//
// Contenido: Implementacion de los metodos de Observador
//
//////////////////////////////////////////////////////////////

#include "Observador.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Observador");

Observador::Observador(Ptr<OnOffApplication> cliente_ptr, 
        Ptr<PacketSink> servidor_ptr) {
    NS_LOG_FUNCTION(this);

    m_cliente_ptr = cliente_ptr;
    m_servidor_ptr = servidor_ptr;

    m_numpacketsTx = 0;
    m_numpacketsRx = 0;

    ProgramaTrazas();
}

void Observador::ProgramaTrazas() {
    NS_LOG_FUNCTION(this);

    m_cliente_ptr->TraceConnectWithoutContext ("Tx",
        MakeCallback(&Observador::clienteTx, this));
    m_servidor_ptr->TraceConnectWithoutContext ("Rx",
        MakeCallback(&Observador::servidorRx, this));
}

/********************************************/
/* CALLBACKS                                */
/********************************************/

void Observador::clienteTx(Ptr<const Packet> pqt) {
    NS_LOG_FUNCTION(this);

    uint64_t uid = pqt->GetUid();
    Time now = Simulator::Now();
    NS_LOG_DEBUG("Paquete " << uid << " Tx en el instante " << now.GetSeconds() << "s");
    m_packetsTx[uid] = now.GetNanoSeconds();

    m_numpacketsTx++;
}

void Observador::servidorRx(Ptr<const Packet> pqt, const Address &address) {
    NS_LOG_FUNCTION(this);

    uint64_t uid = pqt->GetUid();
    // Si el paquete recibido es uno de los transmitidos por m_cliente_ptr
    if(m_packetsTx.find(uid) != m_packetsTx.end()) {
        // Calculo el retardo y lo añado en el acumulador
        // Nota, el retardo de los primeros paquetes es significativamente mayor pues 
        // se tiene que enviar un arp preguntando por la mac del router
        uint64_t retardo = Simulator::Now().GetNanoSeconds() - m_packetsTx[uid];
        NS_LOG_DEBUG("Paquete " << uid << " recibido con retardo " << retardo << "ns");
        m_retardoAcum.Update(retardo);
        // Una vez que ya hemos recibido el paquete, lo eliminamos de los 
        // pendientes de recibir
        m_packetsTx.erase(uid);

        m_numpacketsRx++;
    }
}

/********************************************/
/* GETTERS                                  */
/********************************************/

double Observador::getRetardoAverage() {
    NS_LOG_FUNCTION(this);

    return m_retardoAcum.Avg();
}

uint64_t Observador::getNumPacketsTx() {
    NS_LOG_FUNCTION(this);

    return m_numpacketsTx;
}

uint64_t Observador::getNumPacketsRx() {
    NS_LOG_FUNCTION(this);

    return m_numpacketsRx;
}

