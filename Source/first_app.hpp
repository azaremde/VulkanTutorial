#pragma once

#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "lve_model.hpp"

#include <memory>
#include <vector>

namespace lve {

class FirstApp
{
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	FirstApp();
	~FirstApp();

	FirstApp(const FirstApp&) = delete;
	FirstApp& operator=(FirstApp&) = delete;

	void run();

private:
	void loadModels();

	void createPipelineLayout();
	void createPipeline();
	void createCommandBuffers();
	void drawFrame();

	LveWindow lveWindow{ WIDTH, HEIGHT, "Triangle" };
	LveDevice lveDevice{ lveWindow };
	LveSwapChain lveSwapChain{ lveDevice, lveWindow.getExtent() };

	std::unique_ptr<LvePipeline> lvePipeline;
	VkPipelineLayout pipelineLayout;
	std::vector<VkCommandBuffer> commandBuffers;

	std::unique_ptr<LveModel> lveModel;

	//LvePipeline lvePipeline{ lveDevice, "Shaders/DefaultShader.vert.spv", "Shaders/DefaultShader.frag.spv", LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT) };
};

}