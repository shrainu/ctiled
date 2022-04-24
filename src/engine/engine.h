#pragma once

#include "util/common.h"


// Performance
void engine_calculate_delta_time();

double engine_delta_time();

void engine_calculate_fps();

uint32_t engine_fps();

// Events
void engine_poll_events();