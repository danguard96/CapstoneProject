#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include "ModelMatrixPushConstant.h"
#include "VMath.h"
#include "Collider.h"

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
    Collider* collider;
    Actor();
    Actor(float nThetaRadianRotation, float nGammaRadianRotation, MATH::Vec3 nPosition, MATH::Vec3 nScale, std::string nModel, std::string nTexture, Collider* nCollider):
    thetaRadianRotation(nThetaRadianRotation), gammaRadianRotation(nGammaRadianRotation), position(nPosition), scale(nScale), model(nModel), 
        texture(nTexture), collider(nCollider) {}
    Actor(float nThetaRadianRotation, float nGammaRadianRotation, MATH::Vec3 nPosition, MATH::Vec3 nScale, std::string nModel, std::string nTexture):
    thetaRadianRotation(nThetaRadianRotation), gammaRadianRotation(nGammaRadianRotation), position(nPosition), scale(nScale), model(nModel), 
        texture(nTexture) {}
    void move(MATH::Vec3 newPos) {
        position = newPos;
        if(collider)
            collider->setPosition(newPos);
    }
    void setTheta(float tTheta){
        gammaRadianRotation = tTheta;
    }
    VkImage textureImage;
    VkImageView textureImageView;
    VkSampler imageSampler;
    IndexedBufferMemory modelBufferedMemory;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
};