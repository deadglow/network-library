#pragma once

#define SNET_SERVERNAME_MAX 32
#define SNET_USERNAME_MAX 32

#define SNET_DEFAULT_PORT 3453
#define SNET_MAX_CONNECTIONS 16
// How long in ms the client will spend trying to connect to a server
#define SNET_CONNECTION_TIMEOUT 5000
// Packet history size for players, more should be used for rollback
#define SNET_PLAYER_DATA_HISTORY_SIZE 3
#define SNET_ENTITY_DATA_HISTORY_SIZE 3