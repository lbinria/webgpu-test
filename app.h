#pragma once

#include <GLFW/glfw3.h>

#include <webgpu/webgpu.h>
#ifdef WEBGPU_BACKEND_WGPU
#  include <webgpu/wgpu.h>
#endif // WEBGPU_BACKEND_WGPU

#include <glfw3webgpu.h>

#ifdef __EMSCRIPTEN__
#  include <emscripten.h>
#endif // __EMSCRIPTEN__

#include <iostream>
#include <cassert>
#include <vector>

#include "shader.h"

struct App {

	bool init();
	void loop();
	void clean();
	bool isRunning();

	WGPURenderPipeline createRenderPipeline(WGPUDevice device, WGPUTextureFormat surfaceFormat);
	WGPUComputePipeline createComputePipeline(WGPUDevice device);

	GLFWwindow *window;
	WGPUDevice device;
	WGPUQueue queue;
	WGPUSurface surface;


	WGPUInstance instance;

	WGPUBindGroup computeBindGroup;
	WGPUComputePipeline computePipeline;
	WGPURenderPipeline renderPipeline;
	WGPUBuffer vertexBuffer;
};
