// Mock DebugUtils.h for native testing
#pragma once

// Disable debug output for testing
#define DEBUG 0

#if DEBUG
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

