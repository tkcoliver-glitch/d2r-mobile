#ifndef DM_STAT_BATCH_H
#define DM_STAT_BATCH_H

/* Native CharacterStatsPanel modes: 1 = all, 2 = one, 3 = five.
 * The server remains responsible for applying points and their derived stats. */
static inline int dm_stat_batch_request(int stat, int n) {
    if (stat < 0 || stat > 3 || (n != 1 && n != 5 && n != 10 && n != 100 && n != -1)) return -1;
    return stat << 8 | (n == -1 ? 0 : n);
}
static inline int dm_stat_batch_modes(int n, int available, int modes[24]) {
    if (available <= 0 || dm_stat_batch_request(0, n) < 0) return 0;
    if (n == -1) { modes[0] = 1; return 1; }
    if (n > available) n = available;
    int count = 0;
    while (n >= 5) { modes[count++] = 3; n -= 5; }
    while (n-- > 0) modes[count++] = 2;
    return count;
}
#endif
