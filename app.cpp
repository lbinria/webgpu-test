#include "app.h"
#include "helpers.h"

// Screen dims
const int SCR_WIDTH = 1024;
const int SCR_HEIGHT = 768;

const uint32_t nTri = 100;
const uint32_t nVerts = 3 * nTri; // 3 verts by tri
const uint64_t vertSize = 16; // Vertex layout: vec4<f32> = 4 floats = 16 bytes per-vertex.


WGPURenderPipeline createRenderPipeline(WGPUDevice device, WGPUTextureFormat surfaceFormat) {
	
	WGPUShaderModule shaderModule = Shader::createShaderModule(device, "shaders/render.wgsl");

	// Setup VBO for render shader
	WGPUVertexAttribute attrs[2] = {};
	attrs[0].format = WGPUVertexFormat_Float32x4;
	attrs[0].offset = 0;
	attrs[0].shaderLocation = 0;

	WGPUVertexBufferLayout bufLayout = {};
	bufLayout.arrayStride = vertSize;
	bufLayout.stepMode = WGPUVertexStepMode_Vertex;
	// bufLayout.attributeCount = 2;
	bufLayout.attributeCount = 1;
	bufLayout.attributes = attrs;


	WGPUBlendState blendState{};
	blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
	blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
	blendState.color.operation = WGPUBlendOperation_Add;
	blendState.alpha.srcFactor = WGPUBlendFactor_Zero;
	blendState.alpha.dstFactor = WGPUBlendFactor_One;
	blendState.alpha.operation = WGPUBlendOperation_Add;

	WGPUColorTargetState colorTarget{};
	colorTarget.format = surfaceFormat;
	colorTarget.blend = &blendState;
	colorTarget.writeMask = WGPUColorWriteMask_All;

	WGPURenderPipelineDescriptor pipelineDesc = {};
	pipelineDesc.nextInChain = nullptr;
	pipelineDesc.label = "standard_pipeline_2";
	
	WGPUVertexState vertexState{};
	vertexState.bufferCount = 1;
	vertexState.buffers = &bufLayout;
	vertexState.module = shaderModule;
	vertexState.entryPoint = "vs_main";
	vertexState.constantCount = 0;
	vertexState.constants = nullptr;

	pipelineDesc.vertex = vertexState;

	WGPUFragmentState fragmentState{};
	fragmentState.nextInChain = nullptr;
	fragmentState.module = shaderModule;
	fragmentState.entryPoint = "fs_main";
	fragmentState.constantCount = 0;
	fragmentState.constants = nullptr;
	fragmentState.targetCount = 1;
	fragmentState.targets = &colorTarget;
	pipelineDesc.fragment = &fragmentState;

	pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
	pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
	pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
	pipelineDesc.primitive.cullMode = WGPUCullMode_None;

	pipelineDesc.depthStencil = nullptr;

	// Samples per pixel
	pipelineDesc.multisample.count = 1;
	// Default value for the mask, meaning "all bits on"
	pipelineDesc.multisample.mask = ~0u;
	// Default value as well (irrelevant for count = 1 anyways)
	pipelineDesc.multisample.alphaToCoverageEnabled = false;

	pipelineDesc.layout = nullptr;
	
	WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDesc);

	// No longer needed
	wgpuShaderModuleRelease(shaderModule);

	return pipeline;
}

WGPUComputePipeline createComputePipeline(WGPUDevice device, WGPUBindGroupLayout bgl) {

	WGPUShaderModule shaderModule = Shader::createShaderModule(device, "shaders/compute.wgsl");

	WGPUProgrammableStageDescriptor csDesc{};
	csDesc.entryPoint = "main";
	csDesc.constantCount = 0;
	csDesc.constants = nullptr;
	csDesc.module = shaderModule;


	WGPUPipelineLayoutDescriptor layoutDesc{};
	layoutDesc.bindGroupLayoutCount = 1;
	layoutDesc.bindGroupLayouts = &bgl;

	WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(device, &layoutDesc);

	WGPUComputePipelineDescriptor pipelineDesc{};
	pipelineDesc.compute = csDesc;
	pipelineDesc.layout = pipelineLayout;

	WGPUComputePipeline pipeline = wgpuDeviceCreateComputePipeline(device, &pipelineDesc);

	// No longer needed
	wgpuShaderModuleRelease(shaderModule);

	return pipeline;
}

WGPUTextureView getNextSurfaceTextureView(WGPUSurface surface) {
	// Get the surface texture
	WGPUSurfaceTexture surfaceTexture;
	wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);
	if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_Success) {
		return nullptr;
	}

	// Create a view for this surface texture
	WGPUTextureViewDescriptor viewDescriptor;
	viewDescriptor.nextInChain = nullptr;
	viewDescriptor.label = "Surface texture view";
	viewDescriptor.format = wgpuTextureGetFormat(surfaceTexture.texture);
	viewDescriptor.dimension = WGPUTextureViewDimension_2D;
	viewDescriptor.baseMipLevel = 0;
	viewDescriptor.mipLevelCount = 1;
	viewDescriptor.baseArrayLayer = 0;
	viewDescriptor.arrayLayerCount = 1;
	viewDescriptor.aspect = WGPUTextureAspect_All;
	WGPUTextureView targetView = wgpuTextureCreateView(surfaceTexture.texture, &viewDescriptor);

#ifndef WEBGPU_BACKEND_WGPU
	// We no longer need the texture, only its view
	// (NB: with wgpu-native, surface textures must not be manually released)
	wgpuTextureRelease(surfaceTexture.texture);
#endif // WEBGPU_BACKEND_WGPU

	return targetView;
}

bool App::init() {

	std::cout << "hello" << std::endl;

	uint64_t bufferSize = nVerts * vertSize;

	// We create a descriptor
	WGPUInstanceDescriptor desc = {};
	desc.nextInChain = nullptr;

	// We create the instance using this descriptor
	instance = wgpuCreateInstance(&desc);

	// We can check whether there is actually an instance created
	if (!instance) {
		std::cerr << "Could not initialize WebGPU!" << std::endl;
		return false;
	}

	// Display the object (WGPUInstance is a simple pointer, it may be
	// copied around without worrying about its size).
	std::cout << "WGPU instance: " << instance << std::endl;

	// GLFW !
	if (!glfwInit()) {
		std::cerr << "Could not initialize GLFW!" << std::endl;
		return false;
 	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "webgpu test", nullptr, nullptr);

	if (!window) {
		std::cerr << "Could not create window!" << std::endl;
		glfwTerminate();
		return false;
	}


	// Surface
	// Should keep alive during the app lifetime !
	surface = glfwGetWGPUSurface(instance, window);

	std::cout << "Requesting adapter..." << std::endl;

	WGPURequestAdapterOptions adapterOpts = {};
	adapterOpts.nextInChain = nullptr;
	adapterOpts.compatibleSurface = surface;
	
	WGPUAdapter adapter = requestAdapterSync(instance, &adapterOpts);

	std::cout << "Got adapter: " << adapter << std::endl;

	WGPUSupportedLimits supportedLimits = {};
	supportedLimits.nextInChain = nullptr;
	bool success = wgpuAdapterGetLimits(adapter, &supportedLimits); 

	if (success) {
		std::cout << "Adapter limits:" << std::endl;
		std::cout << " - maxTextureDimension1D: " << supportedLimits.limits.maxTextureDimension1D << std::endl;
		std::cout << " - maxTextureDimension2D: " << supportedLimits.limits.maxTextureDimension2D << std::endl;
		std::cout << " - maxTextureDimension3D: " << supportedLimits.limits.maxTextureDimension3D << std::endl;
		std::cout << " - maxTextureArrayLayers: " << supportedLimits.limits.maxTextureArrayLayers << std::endl;
	}


	WGPUAdapterProperties properties = {};
	properties.nextInChain = nullptr;
	wgpuAdapterGetProperties(adapter, &properties);
	std::cout << "Adapter properties:" << std::endl;
	std::cout << " - vendorID: " << properties.vendorID << std::endl;
	if (properties.vendorName) {
		std::cout << " - vendorName: " << properties.vendorName << std::endl;
	}
	if (properties.architecture) {
		std::cout << " - architecture: " << properties.architecture << std::endl;
	}
	std::cout << " - deviceID: " << properties.deviceID << std::endl;
	if (properties.name) {
		std::cout << " - name: " << properties.name << std::endl;
	}
	if (properties.driverDescription) {
		std::cout << " - driverDescription: " << properties.driverDescription << std::endl;
	}
	std::cout << std::hex;
	std::cout << " - adapterType: 0x" << properties.adapterType << std::endl;
	std::cout << " - backendType: 0x" << properties.backendType << std::endl;
	std::cout << std::dec; // Restore decimal numbers


	std::vector<WGPUFeatureName> features;

	// Call the function a first time with a null return address, just to get
	// the entry count.
	size_t featureCount = wgpuAdapterEnumerateFeatures(adapter, nullptr);

	// Allocate memory (could be a new, or a malloc() if this were a C program)
	features.resize(featureCount);

	// Call the function a second time, with a non-null return address
	wgpuAdapterEnumerateFeatures(adapter, features.data());

	std::cout << "Adapter features:" << std::endl;
	std::cout << std::hex; // Write integers as hexadecimal to ease comparison with webgpu.h literals
	for (auto f : features) {
		std::cout << " - 0x" << f << std::endl;
	}
	std::cout << std::dec; // Restore decimal numbers


	std::cout << "Requesting device..." << std::endl;

	WGPUDeviceDescriptor deviceDesc = {};
	// [...] Build device descriptor
	device = requestDeviceSync(adapter, &deviceDesc);

	std::cout << "Got device: " << device << std::endl;

	deviceDesc.nextInChain = nullptr;
	deviceDesc.label = "My Device"; // anything works here, that's your call
	deviceDesc.requiredFeatureCount = 0; // we do not require any specific feature
	deviceDesc.requiredLimits = nullptr; // we do not require any specific limit
	deviceDesc.defaultQueue.nextInChain = nullptr;
	deviceDesc.defaultQueue.label = "The default queue";
	// [...] Set device lost callback
	// A function that is invoked whenever the device stops being available.
	deviceDesc.deviceLostCallback = [](WGPUDeviceLostReason reason, char const* message, void* /* pUserData */) {
		std::cout << "Device lost: reason " << reason;
		if (message) std::cout << " (" << message << ")";
		std::cout << std::endl;
	};

	auto onDeviceError = [](WGPUErrorType type, char const* message, void* /* pUserData */) {
		std::cout << "Uncaptured device error: type " << type;
		if (message) std::cout << " (" << message << ")";
		std::cout << std::endl;
	};
	wgpuDeviceSetUncapturedErrorCallback(device, onDeviceError, nullptr /* pUserData */);

	inspectDevice(device);

	// Surface config
	WGPUTextureFormat surfaceFormat = wgpuSurfaceGetPreferredFormat(surface, adapter);

	// Seems to be released now...
	wgpuAdapterRelease(adapter);

	WGPUSurfaceConfiguration config = {};
	config.nextInChain = nullptr;
	config.width = SCR_WIDTH;
	config.height = SCR_HEIGHT;
	config.format = surfaceFormat;
	config.viewFormatCount = 0;
	config.viewFormats = nullptr;
	config.usage = WGPUTextureUsage_RenderAttachment;
	config.device = device;
	config.presentMode = WGPUPresentMode_Fifo;
	config.alphaMode = WGPUCompositeAlphaMode_Auto;
	wgpuSurfaceConfigure(surface, &config);



	// Command queue
	queue = wgpuDeviceGetQueue(device);

	auto onQueueWorkDone = [](WGPUQueueWorkDoneStatus status, void* /* pUserData */) {
		std::cout << "Queued work finished with status: " << status << std::endl;
	};
	wgpuQueueOnSubmittedWorkDone(queue, onQueueWorkDone, nullptr /* pUserData */);



	// Render pipeline
	renderPipeline = createRenderPipeline(device, surfaceFormat);

	// Compute pipeline
	WGPUBindGroupLayoutEntry layoutEntry{};
	layoutEntry.binding = 0;
	layoutEntry.visibility = WGPUShaderStage_Compute;
	layoutEntry.buffer.type = WGPUBufferBindingType_Storage;
	layoutEntry.buffer.hasDynamicOffset = false;
	layoutEntry.buffer.minBindingSize = bufferSize;

	WGPUBindGroupLayoutDescriptor bglDesc{};
	bglDesc.entryCount = 1;
	bglDesc.entries = &layoutEntry;
	WGPUBindGroupLayout bgl = wgpuDeviceCreateBindGroupLayout(device, &bglDesc);

	// Create buffer
	WGPUBufferDescriptor bufDesc{};
	bufDesc.usage = WGPUBufferUsage_Storage | WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
	bufDesc.size = bufferSize;
	bufDesc.label = "vertex_storage_buffer";
	vertexBuffer = wgpuDeviceCreateBuffer(device, &bufDesc);

	WGPUBindGroupEntry bgEntry{};
	bgEntry.binding = 0;
	bgEntry.buffer = vertexBuffer;
	bgEntry.offset = 0;
	bgEntry.size = bufferSize;

	WGPUBindGroupDescriptor bgDesc{};
	bgDesc.entryCount = 1;
	bgDesc.entries = &bgEntry;
	bgDesc.layout = bgl;
	computeBindGroup = wgpuDeviceCreateBindGroup(device, &bgDesc);


	computePipeline = createComputePipeline(device, bgl);

	return true;
}

void App::loop() {

	glfwPollEvents();
	
	if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
	}
	
	auto targetView = getNextSurfaceTextureView(surface);
	if (!targetView)
		return;


	// Because the compute pass that wrote the buffer is encoded before the render pass on the same command encoder and submitted together, 
	// GPU execution ordering guarantees the compute writes are visible to the render pass (no extra barrier needed in this case).

	// Command encoder
	WGPUCommandEncoderDescriptor encoderDesc = {};
	encoderDesc.nextInChain = nullptr;
	encoderDesc.label = "command_encoder";
	WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);


	// Compute pass
	WGPUComputePassDescriptor cpEncDesc = {};
	WGPUComputePassEncoder computePass = wgpuCommandEncoderBeginComputePass(encoder, &cpEncDesc);
	wgpuComputePassEncoderSetPipeline(computePass, computePipeline);
	wgpuComputePassEncoderSetBindGroup(computePass, 0, computeBindGroup, 0, nullptr);
	wgpuComputePassEncoderDispatchWorkgroups(computePass, nTri, 1, 1); // 100 invocations
	wgpuComputePassEncoderEnd(computePass);

	// Render pass
	WGPURenderPassDescriptor renderPassDesc = {};
	renderPassDesc.nextInChain = nullptr;

	WGPURenderPassColorAttachment renderPassColorAttachment = {};
	renderPassColorAttachment.nextInChain = nullptr;
	renderPassColorAttachment.resolveTarget = nullptr;
	renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
	renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
	renderPassColorAttachment.clearValue = {0., 0., 0.};
	#ifndef WEBGPU_BACKEND_WGPU
	renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
	#endif // NOT WEBGPU_BACKEND_WGPU
	renderPassColorAttachment.view = targetView;

	// Setup render pass
	renderPassDesc.colorAttachmentCount = 1;
	renderPassDesc.colorAttachments = &renderPassColorAttachment;
	renderPassDesc.depthStencilAttachment = nullptr;
	renderPassDesc.timestampWrites = nullptr;

	WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);

	wgpuRenderPassEncoderSetPipeline(renderPass, renderPipeline);
	wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, vertexBuffer, 0, WGPU_WHOLE_SIZE);
	wgpuRenderPassEncoderDraw(renderPass, nVerts, 1, 0, 0);
	wgpuRenderPassEncoderEnd(renderPass);

	wgpuRenderPassEncoderRelease(renderPass);


	// Finally encode and submit the command
	WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
	cmdBufferDescriptor.nextInChain = nullptr;
	cmdBufferDescriptor.label = "Command buffer";
	WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
	wgpuCommandEncoderRelease(encoder);

	wgpuQueueSubmit(queue, 1, &command);
	wgpuCommandBufferRelease(command);

	// At the end of the frame
	wgpuTextureViewRelease(targetView);

	// We present surface to the screen
	#ifndef __EMSCRIPTEN__
	wgpuSurfacePresent(surface);
	#endif

	#if defined(WEBGPU_BACKEND_DAWN)
		wgpuDeviceTick(device);
	#elif defined(WEBGPU_BACKEND_WGPU)
		wgpuDevicePoll(device, false, nullptr);
	#endif
}

bool App::isRunning() {
	return !glfwWindowShouldClose(window);
}

void App::clean() {
	// Clean up
	glfwDestroyWindow(window);
	glfwTerminate();
	wgpuRenderPipelineRelease(renderPipeline);
	wgpuSurfaceUnconfigure(surface);
	wgpuSurfaceRelease(surface);
	wgpuQueueRelease(queue);
	wgpuDeviceRelease(device);
	wgpuInstanceRelease(instance);
}