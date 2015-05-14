// vim:ts=4:sw=4:expandtab
#pragma once

/**
 * Set up fake RandR outputs from a string.
 */
void fake_outputs_create_outputs(char *outputs_str);

/**
 * Open marker windows for the fake outputs.
 */
void fake_outputs_visualize(void);
