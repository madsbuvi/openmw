#ifndef OPENMW_WORLDPACKET_HPP
#define OPENMW_WORLDPACKET_HPP

#include <string>
#include <RakNetTypes.h>
#include <BitStream.h>
#include <PacketPriority.h>
#include <components/openmw-mp/Base/BaseEvent.hpp>

#include <components/openmw-mp/Packets/BasePacket.hpp>


namespace mwmp
{
    class WorldPacket : public BasePacket
    {
    public:
        WorldPacket(RakNet::RakPeerInterface *peer);

        ~WorldPacket();

        virtual void Packet(RakNet::BitStream *bs, BaseEvent *event, bool send);

        virtual void Send(BaseEvent *event, bool toOtherPlayers = true);
        virtual void Send(BaseEvent *event, RakNet::AddressOrGUID destination);
        virtual void Read(BaseEvent *event);

        static size_t headerSize()
        {
            return (size_t)(1 + RakNet::RakNetGUID::size()); // packetID + RakNetGUID (uint64_t)
        }

        unsigned char GetPacketID()
        {
            return packetID;
        }

    protected:
        BaseEvent *event;

    };
}

#endif //OPENMW_WORLDPACKET_HPP
