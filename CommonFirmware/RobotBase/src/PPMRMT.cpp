#include <Arduino.h>

// The RMT (Remote Control) module library is used for generating the DShot signal.
#include <driver/rmt_rx.h>
#include <driver/rmt_tx.h>

//less universal now, but can be undone if I *really* need it for something else...
#include "configuration.h"
#include "PPMRMT.h"


