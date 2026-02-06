#pragma once

#include <esp_log.h>

#define LOG_TAG "Proofi"

// Simple logging wrappers using ESP-IDF logging
static inline void debug_log(const char* v) { ESP_LOGI(LOG_TAG, "%s", v); }
static inline void debug_log(int v) { ESP_LOGI(LOG_TAG, "%d", v); }
static inline void debug_log(long v) { ESP_LOGI(LOG_TAG, "%ld", v); }
static inline void debug_log(unsigned long v) { ESP_LOGI(LOG_TAG, "%lu", v); }
static inline void debug_log(float v) { ESP_LOGI(LOG_TAG, "%f", (double)v); }
static inline void debug_log(double v) { ESP_LOGI(LOG_TAG, "%f", v); }
static inline void debug_log(bool v) { ESP_LOGI(LOG_TAG, "%s", v ? "true" : "false"); }

#define DEBUG_PRINT(x) debug_log(x)
#define DEBUG_PRINTLN(x) debug_log(x)
