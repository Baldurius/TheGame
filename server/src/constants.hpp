#pragma once

enum class ServerMessage : uint32_t
{
    LOGIN = 0x00000000,
};

enum class ClientMessage : uint32_t
{
    GAME_START = 0x00000001,
    HELLO = 0x00000000,
};
