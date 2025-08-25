// Thin entrypoint delegating to App orchestrator
#include <Arduino.h>
#include "app/App.h"

app::App APP;

void setup() { APP.setup(); }
void loop()  { APP.loopTick(); }
