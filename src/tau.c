#include "support.h"

// This is the global TAU singleton, which represents the current
// simulated time in undefined units with undefined zero point.

// TAU is set by "stepat" to the TAU for the step being run.

Tau                 TAU = 0;
