#include "Pipeline.hpp"

#include "AstrumVK/Models/Vertex.hpp"

void Pipeline::setShaderStages()
{
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};

    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = shader.getVertShaderModule();
    vertShaderStageInfo.pName = "main";

    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = shader.getFragShaderModule();
    fragShaderStageInfo.pName = "main";

    shaderStages = {
        vertShaderStageInfo, fragShaderStageInfo
    };

    DebugLogOut("[Graphics pipeline]: Shader stages loaded.");
}

void Pipeline::createDescriptorSetLayout()
{    
    VkDescriptorSetLayoutBinding dynamicBinding0{};
    dynamicBinding0.binding = 0;
    dynamicBinding0.descriptorCount = 1;
    dynamicBinding0.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    dynamicBinding0.pImmutableSamplers = nullptr;
    dynamicBinding0.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding staticBinding1{};
    staticBinding1.binding = 1;
    staticBinding1.descriptorCount = 1;
    staticBinding1.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    staticBinding1.pImmutableSamplers = nullptr;
    staticBinding1.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    std::vector<VkDescriptorSetLayoutBinding> bindings = { dynamicBinding0, staticBinding1 };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();
    
    VK_CHECK(
        vkCreateDescriptorSetLayout(gpu.getDevice(), &layoutInfo, nullptr, &descriptorSetLayout),
        "Failed to create descriptor set layout."
    );
}

void Pipeline::destroyDescriptorSetLayout()
{    
    vkDestroyDescriptorSetLayout(gpu.getDevice(), descriptorSetLayout, nullptr);
}

void Pipeline::Fixed::setVertexInputInfo()
{    
    bindings = Vertex::getBindingDescription();
    attributes = Vertex::getAttributeDescriptions();

    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());

    vertexInputInfo.pVertexBindingDescriptions = &bindings; // Optional
    vertexInputInfo.pVertexAttributeDescriptions = attributes.data(); // Optional
}

void Pipeline::Fixed::setInputAssemblyInfo()
{
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
}

void Pipeline::Fixed::setViewportInfo(const VkExtent2D& extent)
{    
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
}

void Pipeline::Fixed::setScissorInfo(const VkExtent2D& extent)
{    
    scissor.offset = {0, 0};
    scissor.extent = extent;
}

void Pipeline::Fixed::setViewportState()
{
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
}

void Pipeline::Fixed::setRasterizerState()
{
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;

    // rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    // rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
}

void Pipeline::Fixed::setMultisamplingState()
{
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional
}

void Pipeline::Fixed::setColorBlendAttachment()
{
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
}

void Pipeline::Fixed::setColorBlending()
{    
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional
}

void Pipeline::Fixed::setDynamicStates()
{    
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;
}

void Pipeline::createPipelineLayout()
{    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    pipelineLayoutInfo.setLayoutCount = 1; // Optional
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout; // Optional

    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    VK_CHECK(
        vkCreatePipelineLayout(gpu.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout),
        "Failed to create pipeline layout."
    );
}

void Pipeline::destroyPipelineLayout()
{
    vkDestroyPipelineLayout(gpu.getDevice(), pipelineLayout, nullptr);
}

void Pipeline::createRenderPass()
{
    renderPass = new RenderPass(gpu, swapChain);
}

void Pipeline::destroyRenderPass()
{
    delete renderPass;
}

void Pipeline::createPipeline()
{
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &fixed.vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &fixed.inputAssembly;
    pipelineInfo.pViewportState = &fixed.viewportState;
    pipelineInfo.pRasterizationState = &fixed.rasterizer;
    pipelineInfo.pMultisampleState = &fixed.multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &fixed.colorBlending;
    pipelineInfo.pDynamicState = nullptr; // Optional
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass->getRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    VK_CHECK(
        vkCreateGraphicsPipelines(gpu.getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline),
        "Failed to create graphics pipeline."
    );

    DebugLogOut("Graphics pipeline created.");
}

void Pipeline::destroyPipeline()
{
    vkDestroyPipeline(gpu.getDevice(), pipeline, nullptr);
}

void Pipeline::createGraphicsPipeline()
{
    setShaderStages();

    createDescriptorSetLayout();

    fixed.setVertexInputInfo();
    fixed.setInputAssemblyInfo();
    fixed.setViewportInfo(swapChain.getExtent());
    fixed.setScissorInfo(swapChain.getExtent());
    fixed.setViewportState();
    fixed.setRasterizerState();
    fixed.setMultisamplingState();
    fixed.setColorBlendAttachment();
    fixed.setColorBlending();
    fixed.setDynamicStates();
    
    createPipelineLayout();
    createRenderPass();
    createPipeline();
}

void Pipeline::destroyGraphicsPipeline()
{
    destroyDescriptorSetLayout();
    destroyPipeline();
    destroyRenderPass();
    destroyPipelineLayout();

    DebugLogOut("Graphics pipeline destroyed.");
}

Pipeline::Pipeline(GPU& _gpu, SwapChain& _swapChain, Shader& _shader) : gpu { _gpu }, swapChain { _swapChain }, shader { _shader }
{
    createGraphicsPipeline();
}

Pipeline::~Pipeline()
{
    destroyGraphicsPipeline();
}

const VkPipeline& Pipeline::getPipeline() const
{
    return pipeline;
}

const VkRenderPass& Pipeline::getRenderPass() const
{
    return renderPass->getRenderPass();
}

const VkPipelineLayout& Pipeline::getPipelineLayout() const
{
    return pipelineLayout;
}

const VkDescriptorSetLayout& Pipeline::getDescriptorSetLayout() const
{
    return descriptorSetLayout;
}