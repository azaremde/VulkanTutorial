#include "Framebuffer.hpp"

Framebuffer::Framebuffer(GPU& _gpu, SwapChain& _swapChain, RenderPass& _renderPass) : gpu{ _gpu }, swapChain{ _swapChain }, renderPass { _renderPass }
{
    const std::vector<VkImageView>& swapChainImageViews = swapChain.GetSwapChainImageViews();

	swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        VkImageView attachments[] = {
            swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass.GetRenderPass();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChain.GetSwapChainExtent().width;
        framebufferInfo.height = swapChain.GetSwapChainExtent().height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(gpu.Device(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

Framebuffer::~Framebuffer()
{
    for (const auto& framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(gpu.Device(), framebuffer, nullptr);
    }
}

const std::vector<VkFramebuffer>& Framebuffer::GetSwapChainFramebuffers() const
{
    return swapChainFramebuffers;
}