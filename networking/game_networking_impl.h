#ifndef GAME_NETWORKING_IMPL_H
#define GAME_NETWORKING_IMPL_H

#include "game_networking.h"

namespace game_networking_impl
{

using namespace game_networking;

bool send_message( ENetPeer *peer, ENetHost *host, const unsigned char *message, const unsigned message_length, const message_channel channel );

} // end namespace game_networking_impl

#endif // GAME_NETWORKING_IMPL_H

