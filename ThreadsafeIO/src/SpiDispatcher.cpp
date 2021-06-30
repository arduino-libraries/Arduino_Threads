#include "SpiDispatcher.h"

rtos::Queue<IoRequest, 32> _request_queue;
