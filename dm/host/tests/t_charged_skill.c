// A stale item binding must resolve to the unique equipped charge source.
// Multiple sources must not silently spend charges from an arbitrary item.
#include "../dm_charged_skill.h"
#include <stdio.h>
#include <string.h>

typedef struct { uint64_t item[20], stats[40], entry; } fixture;
static void init(fixture *f, int id, int skill, int charges, int extended) {
    memset(f, 0, sizeof *f);
    *(uint32_t *)((uint8_t *)f->item + 8) = (uint32_t)id;
    *(uint64_t *)((uint8_t *)f->item + 0x88) = (uint64_t)f->stats;
    *(int32_t *)((uint8_t *)f->stats + 0x1c) = extended ? (int32_t)0x80000000u : 0;
    uint8_t *a = (uint8_t *)f->stats + (extended ? 0xe8 : 0x30);
    *(uint64_t *)a = (uint64_t)&f->entry; *(uint64_t *)(a + 8) = 1;
    f->entry = (uint64_t)(uint32_t)((70 << 8) | charges) << 32 | (204u << 16) | (unsigned)(skill << 6) | 1u;
}
static int failures;
static void check(const char *what, int want, int got) {
    printf("%s: expected %d, got %d %s\n", what, want, got, want == got ? "PASS" : "FAIL");
    failures += want != got;
}
int main(void) {
    fixture old, teleport, resist, other;
    init(&old, 74, 10, 8, 0); init(&teleport, 58, 54, 33, 1);
    init(&resist, 60, 91, 12, 0); init(&other, 79, 54, 9, 1);
    uint64_t worn[] = {(uint64_t)old.item, (uint64_t)teleport.item, (uint64_t)resist.item, (uint64_t)other.item};
    check("teleport native remaining charges", 33, dm_charged_skill_count(worn[1], 54));
    check("lower resist native remaining charges", 12, dm_charged_skill_count(worn[2], 91));
    check("stale teleport source", 58, dm_charged_skill_source(worn, 3, 54, 74));
    check("stale lower resist source", 60, dm_charged_skill_source(worn, 3, 91, 74));
    check("valid source wins despite second provider", 58, dm_charged_skill_source(worn, 4, 54, 58));
    check("ambiguous source preserves binding", 74, dm_charged_skill_source(worn, 4, 54, 74));
    check("unequipped provider is excluded", 74, dm_charged_skill_source(worn, 1, 54, 74));
    check("learned skill is excluded", -1, dm_charged_skill_source(worn, 3, 54, -1));
    init(&teleport, 58, 54, 0, 1);
    check("empty charges still identify the item", 58, dm_charged_skill_source(worn, 3, 54, 74));
    return failures != 0;
}
