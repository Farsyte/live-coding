#include <assert.h>     // Conditionally compiled macro that compares its argument to zero
#include "common/support.h"

void tau_post()
{
    // Tau values need to be more than 4 bytes.
    assert(sizeof(Tau) > 4);
}
