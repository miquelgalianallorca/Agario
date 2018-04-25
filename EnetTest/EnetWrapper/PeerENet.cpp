#include "PeerENet.h"
#include "Random.h"

#include "enet2/enet.h"

static CRandom RandomGenerator = CRandom();

namespace ENet 
{

CPeerENet::CPeerENet(ENetPeer* pPeer, float _fLatencyBase, float _fLatencyJitter, float _fPacketDropChance)
{
    m_pPeer = pPeer;
    m_fLatencyBase = _fLatencyBase;
    m_fLatencyJitter = _fLatencyJitter;
    m_fPacketDropChance = _fPacketDropChance;
}

// PHORNEE: A�adido service para gestionar la latencia de los paquetes
void CPeerENet::Service(float dt)
{
    std::list<TPendingPacket>::iterator iter = PendingPackets.begin();

    for (; iter != PendingPackets.end();)
    {
        TPendingPacket PendingPacket = *iter;
        iter->fRemainingLatency -= dt;
        if (iter->fRemainingLatency <= 0.f)
        {
            enet_peer_send(GetENetPeer(), iter->channel, iter->pPacket);
            iter = PendingPackets.erase(iter);
        }
        else
            iter++;
    }
}

// Para a�adir simulaci�n de latencia a bajo nivel
void CPeerENet::SendPacket(ENetPacket* packet, int channel)
{
    // Los paquetes reliables se envian sin latencia para que lleguen en orden, porque si los desordenamos
    // antes de pasarselos a enet, enet conservar� dicho desorden, y eso no lo queremos.
    // Lo ideal ser�a implementar la latencia dentro del motor de enet.
    if ((packet->flags & ENET_PACKET_FLAG_RELIABLE) != 0)
    {
        enet_peer_send(GetENetPeer(), channel, packet);
    }
    else
    {
        if (RandomGenerator.GetRandFloat(0.0f, 1.0f) >= m_fPacketDropChance)
        {
            float fLatency= m_fLatencyBase + RandomGenerator.GetRandFloat(-m_fLatencyJitter, m_fLatencyJitter);
            PendingPackets.push_back(TPendingPacket(packet, channel, fLatency));
        }
        else
        {
            fprintf(stdout, "Packet lost (simulated).");
        }
    }
}

} //namespace ENet