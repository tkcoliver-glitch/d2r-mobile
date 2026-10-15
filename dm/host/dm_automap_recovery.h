#pragma once
#include <stdatomic.h>

typedef struct { _Atomic int phase; } dm_automap_recovery;
static inline void dm_automap_movie_begin(dm_automap_recovery *s, int preserve_open_map) {
    atomic_store(&s->phase, preserve_open_map ? 1 : 0);
}
static inline void dm_automap_movie_complete(dm_automap_recovery *s) {
    int armed = 1; atomic_compare_exchange_strong(&s->phase, &armed, 2);
}
static inline int dm_automap_restore_take(dm_automap_recovery *s, int touch, int ready, int exists, int visible) {
    if (!touch) { atomic_store(&s->phase, 0); return 0; }
    if (!ready || !exists) return 0;
    int pending = 2;
    return atomic_compare_exchange_strong(&s->phase, &pending, 0) && !visible;
}
