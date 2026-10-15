#pragma once
#include <stdint.h>

// Native item stat 204: layer = skill << 6 | level, value = max << 8 | remaining.
static inline int dm_charged_skill_count(uint64_t item, int skill) {
    uint64_t sl = *(uint64_t *)(item + 0x88); if (!sl) return -1;
    uint64_t a = (*(int32_t *)(sl + 0x1c) < 0) ? sl + 0xe8 : sl + 0x30;
    uint64_t arr = *(uint64_t *)a, count = *(uint64_t *)(a + 8);
    for (uint64_t i = 0; arr && i < count && i < 512; i++) {
        uint32_t key = *(uint32_t *)(arr + 8 * i);
        if ((key >> 16) == 204 && (int)((key & 0xffff) >> 6) == skill)
            return *(int32_t *)(arr + 8 * i + 4) & 0xff;
    }
    return -1;
}

static inline int32_t dm_charged_skill_source(const uint64_t *equipped, int count, int skill, int32_t bound) {
    if (bound < 0) return bound;
    int32_t candidate = -1; int ambiguous = 0;
    for (int i = 0; i < count; i++) {
        if (dm_charged_skill_count(equipped[i], skill) < 0) continue;
        int32_t id = *(int32_t *)(equipped[i] + 8);
        if (id == bound) return bound;
        if (candidate >= 0 && candidate != id) ambiguous = 1;
        candidate = id;
    }
    return candidate >= 0 && !ambiguous ? candidate : bound;
}
