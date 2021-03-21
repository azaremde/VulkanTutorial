#pragma once

#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"

namespace lve {

class FirstApp
{
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	void run();

private:
	LveWindow lveWindow{ WIDTH, HEIGHT, "Triangle" };
	LveDevice lveDevice{ lveWindow };
	LvePipeline lvePipeline{ lveDevice, "Shaders/DefaultShader.vert.spv", "Shaders/DefaultShader.frag.spv", LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT) };
};

}