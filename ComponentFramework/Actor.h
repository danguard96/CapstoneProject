#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include "ModelMatrixPushConstant.h"
#include "VMath.h"

struct BufferMemory {
    VkBuffer bufferID;
    VkDeviceMemory bufferMemoryID;
};

struct IndexedBufferMemory {
    VkBuffer vertBufferID;
    VkDeviceMemory vertBufferMemoryID;
    VkDeviceSize vertBufferSize;

    VkBuffer indexBufferID;
    VkDeviceMemory indexBufferMemoryID;
    VkDeviceSize indexBufferSize;
};

class Actor{
public:
    float thetaRadianRotation;
    float gammaRadianRotation;
    MATH::Vec3 position;
    ModelMatrixPushConst modelMatrixPushConst;
    MATH::Vec3 scale;
    std::string model;
    std::string texture;
    float colliderRadius;
    Actor(float nThetaRadianRotation, float nGammaRadianRotation, MATH::Vec3 nPosition, MATH::Vec3 nScale, std::string nModel, std::string nTexture, float nColliderRadius):
    thetaRadianRotation(nThetaRadianRotation), gammaRadianRotation(nGammaRadianRotation), position(nPosition), scale(nScale), model(nModel), 
        texture(nTexture), colliderRadius(nColliderRadius) {}
    VkImage textureImage;
    VkImageView textureImageView;
    VkSampler imageSampler;
    IndexedBufferMemory modelBufferedMemory;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
};