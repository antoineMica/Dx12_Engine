#pragma once


enum {
	MAX_INSTANCE_EXTENSIONS = 1024,
	MAX_DEVICE_EXTENSIONS = 1024,
	MAX_GPUS = 4,
	MAX_DESCRIPTORS = 32,
	MAX_DESCRIPTOR_SETS = 8,				// for now only one descriptor set can be bound -> need to implement several descriptor sets
	MAX_RENDER_TARGET_ATTACHMENTS = 8,
	MAX_SUBMIT_CMDS = 20,					// max number of command lists / command buffers
	MAX_SUBMIT_WAIT_SEMAPHORES = 8,
	MAX_SUBMIT_SIGNAL_SEMAPHORES = 8,
	MAX_PRESENT_WAIT_SEMAPHORES = 8,
	MAX_VERTEX_BINDINGS = 15,
	MAX_VERTEX_ATTRIBS = 15,
	MAX_SEMANTIC_NAME_LENGTH = 128,
	MAX_DESCRIPTOR_ENTRIES = 8192,			// number of buffers / textures / samplers in the pointer array in the descriptor
	MAX_SAMPLER_DESCRIPTOR_ENTRIES = 2048,			// number of buffers / textures / samplers in the pointer array in the descriptor
	MAX_MIP_LEVELS = 0xFFFFFFFF,
	MAX_BUFFER_BINDINGS = 16,
	MAX_TEXTURE_BINDINGS = 32,
	MAX_SAMPLER_BINDINGS = 16
};
