#ifndef __AstrumVK_Tools_Tools_hpp__
#define __AstrumVK_Tools_Tools_hpp__ 

#pragma once

#include "Pch.hpp"

namespace vkt {
namespace renderPass {

struct AttachmentOps
{
    VkAttachmentLoadOp loadOp;
    VkAttachmentStoreOp storeOp;
};

inline static VkAttachmentDescription createAttachmentDescription(
    VkFormat format, 
    VkSampleCountFlagBits samples, 
    
    AttachmentOps ops,

    AttachmentOps stencilOps,

    std::array<VkImageLayout, 2> layouts
)
{
    VkAttachmentDescription colorAttachment{};

    colorAttachment.format = format;
    colorAttachment.samples = samples;

    colorAttachment.loadOp = ops.loadOp;
    colorAttachment.storeOp = ops.storeOp;

    colorAttachment.stencilLoadOp = stencilOps.loadOp;
    colorAttachment.stencilStoreOp = stencilOps.storeOp;

    colorAttachment.initialLayout = layouts[0];
    colorAttachment.finalLayout = layouts[1];

    return colorAttachment;
}

inline static VkAttachmentReference createAttachmentReference(uint32_t attachment, VkImageLayout layout)
{
    VkAttachmentReference ref{};
    ref.attachment = attachment;
    ref.layout = layout;
    return ref;
}

inline static VkSubpassDescription createSubpassDescription(
    VkPipelineBindPoint pipelineBindPoint,
    const std::vector<VkAttachmentReference>& colorAttachmentRefs,
    const std::vector<VkAttachmentReference>& depthAttachmentRefs
)
{    
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = pipelineBindPoint;
    subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefs.size());
    subpass.pColorAttachments = colorAttachmentRefs.data();
    subpass.pDepthStencilAttachment = depthAttachmentRefs.data();
    return subpass;
}

struct Subpasses
{
    uint32_t src;
    uint32_t dst;
};

struct StageMasks
{
    VkPipelineStageFlags src;
    VkPipelineStageFlags dst;
};

struct AccessMasks
{
    VkAccessFlags src;
    VkAccessFlags dst;
};

inline static VkSubpassDependency createSubpassDependency(Subpasses subpasses, StageMasks stageMasks, AccessMasks accessMasks)
{
    VkSubpassDependency dependency;

    dependency.srcSubpass = subpasses.src;
    dependency.dstSubpass = subpasses.dst;

    dependency.srcStageMask = stageMasks.src;
    dependency.dstStageMask = stageMasks.dst;

    dependency.srcAccessMask = accessMasks.src;
    dependency.dstAccessMask = accessMasks.dst;

    return dependency;
}

inline static VkRenderPassCreateInfo createRenderPassCreateInfo(
    const std::vector<VkAttachmentDescription>& attachments,
    const std::vector<VkSubpassDescription>& subpasses,
    const std::vector<VkSubpassDependency>& dependencies
)
{
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();

    renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
    renderPassInfo.pSubpasses = subpasses.data();

    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();

    return renderPassInfo;
}

}
}

#endif