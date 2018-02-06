/* util.c
 *
 * This file is part of fizmo-bot.  Please see LICENSE.md for the license.
 */

// #include <string.h>
#include <stdio.h>
#include "config.h"

void dbg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}
