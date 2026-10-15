// First-clear act movies close AutoMap. Reopen only the previously open map,
// after MovieComplete and after the loading/panel transition has finished.
#include "../dm_automap_recovery.h"
#include <stdio.h>
static int failures;
static void check(const char *what, int want, int got) {
    printf("%s: expected %d, got %d %s\n", what, want, got, want == got ? "PASS" : "FAIL");
    failures += want != got;
}
int main(void) {
    dm_automap_recovery s = {0};
    dm_automap_movie_begin(&s, 1);
    check("keep map hidden during movie", 0, dm_automap_restore_take(&s, 1, 1, 1, 0));
    dm_automap_movie_complete(&s);
    check("wait for loading and panels", 0, dm_automap_restore_take(&s, 1, 0, 1, 0));
    check("wait for recreated map widget", 0, dm_automap_restore_take(&s, 1, 1, 0, 0));
    check("restore after act movie", 1, dm_automap_restore_take(&s, 1, 1, 1, 0));
    check("restore at most once", 0, dm_automap_restore_take(&s, 1, 1, 1, 0));
    dm_automap_movie_begin(&s, 1); dm_automap_movie_complete(&s);
    check("already reopened by game", 0, dm_automap_restore_take(&s, 1, 1, 1, 1));
    check("do not reopen a later manual close", 0, dm_automap_restore_take(&s, 1, 1, 1, 0));
    dm_automap_movie_begin(&s, 0); dm_automap_movie_complete(&s);
    check("closed map / menu movie stays closed", 0, dm_automap_restore_take(&s, 1, 1, 1, 0));
    dm_automap_movie_begin(&s, 1); dm_automap_movie_complete(&s);
    check("controller takes over", 0, dm_automap_restore_take(&s, 0, 1, 1, 0));
    check("touch does not revive old request", 0, dm_automap_restore_take(&s, 1, 1, 1, 0));
    return failures != 0;
}
