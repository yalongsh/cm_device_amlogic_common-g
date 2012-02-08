#include <stdlib.h>
#include <telephony/ril.h>
#include <assert.h>
#include "aml-ril.h"

#define LOG_TAG "AML_RIL"
#include <utils/Log.h>
#include "debug.h"

/* Handler functions. The names are because we cheat by including
 * ril_commands.h from rild. In here we generate local allocations
 * of the data representations, as well as free:ing them after
 * they've been handled.
 *
 * This design might not be ideal, but considering the alternatives,
 * it's good enough.
 */

