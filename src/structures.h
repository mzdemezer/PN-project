#ifndef _INCLUDE_STRUCTURES_H
#define _INCLUDE_STRUCTURES_H

#define SCREEN_BUFFER_WIDTH 1000
#define SCREEN_BUFFER_HEIGHT 750
#define ZONE_FACTOR 50
#define ZONE_SIZE (SCREEN_BUFFER_HEIGHT / ZONE_FACTOR)

#define NumOfThreads 3

#include "structures/RBtree.h"
#include "structures/collstructures.h"
#include "structures/fastreadset.h"
#include "structures/zone.h"

#include "structures/gameobjects.h"

#include "structures/level.h"


#endif
