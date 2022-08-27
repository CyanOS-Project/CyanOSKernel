#pragma once

#include "buffer_view.h"
#include "types.h"

u16 checksum(const BufferView& data);
u16 network_word16(u16 value);
u32 network_word32(u32 value);