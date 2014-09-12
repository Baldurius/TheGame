#pragma once

enum class CMsg : uint32_t
{
    HELLO       = 0x00000001,
    GAME_START  = 0x00000002
};

enum class SMsg : uint32_t
{
    LOGIN       = 0x00000001,
    CHAT        = 0x00000002
};
