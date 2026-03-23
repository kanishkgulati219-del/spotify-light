#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include <lighting/lighting_engine.h>
#include <lighting_types.h>

void protocol_init(void); // initialize parser state.
void protocol_process_frame(volatile uint8_t *frame, uint8_t length); // full pipeline: parse → create event → push to queue.

#endif	//PARSER_H
