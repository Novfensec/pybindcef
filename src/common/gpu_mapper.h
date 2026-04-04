#pragma once
#include <stdint.h>

int platform_map_gpu_texture(uint64_t handle_id, int target_gl_id, int width, int height);
void init_graphics_bridge();
void lock_texture();
void unlock_texture();