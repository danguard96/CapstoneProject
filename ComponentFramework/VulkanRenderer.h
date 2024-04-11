#ifndef VULKANRENDERER_H 
#define VULKANRENDERER_H

#include <SDL.h>
#include <SDL_vulkan.h>
#include <SDL_image.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <unordered_map>
#include <array>
#include <chrono>

#include "VMath.h"
#include "MMath.h"
#include "Hash.h"
#include "Actor.h"
#include "GlobalLighting.h"
#include "NetworkManager.h"
#include "Collider.h"
#include "PlayerInfo.h"
using namespace MATH;

#include "Renderer.h"
#include "CameraPosition.h"

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG /// only use validation layers if in debug mode
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
    /// optional means that it contains no value until it is assigned
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


struct CameraUBO {
    Matrix4 view;
    Matrix4 proj;
    Vec3 viewPos;
};

struct NormalUBO {
    float plength;
    Vec4 colour;
};

struct Vertex {
    Vec3 pos;
    Vec3 normal;
    Vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }

    bool operator == (const Vertex& other) const {
        return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
    }

}; /// End of struct Vertex

/// This is used to generate a hash 
namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const noexcept {
            size_t hash1 = hash<Vec3>()(vertex.pos);
            size_t hash2 = hash<Vec3>()(vertex.normal);
            size_t hash3 = hash<Vec2>()(vertex.texCoord);
            size_t result = ((hash1 ^ (hash2 << 1)) >> 1) ^ (hash3 << 1);
            return result;
        }
    };
}

class VulkanRenderer : public Renderer {
public:
    /// C11 precautions 
    VulkanRenderer(const VulkanRenderer&) = delete;  /// Copy constructor
    VulkanRenderer(VulkanRenderer&&) = delete;       /// Move constructor
    VulkanRenderer& operator=(const VulkanRenderer&) = delete; /// Copy operator
    VulkanRenderer& operator=(VulkanRenderer&&) = delete;      /// Move operator

    VulkanRenderer();
    ~VulkanRenderer();

    SDL_Window* CreateWindow(std::string name_, int width, int height);
    bool OnCreate();
    void OnDestroy();
    void Render();
    void SetCameraUBO(const Matrix4& projection_, const Matrix4& view_, const Vec3& viewPosi);
    void SetGLightsUBO(const GlobalLighting& glightsUBO_);
    void SetNormalUBO(const NormalUBO& normalUBO_);
    void SetSphereModelMatrixPush(Actor* actor, const Matrix4& modelMatrix);
    void SetModelMatrixPush(Actor* actor, const Matrix4& modelMatrix);
    void SetTextureIndex(int textureIndex);
    void commitFrame();
    SDL_Window* GetWindow() { return window; }
    Vec3 cPos = Vec3{0,-1.2,-5};
    NetworkManager nm = NetworkManager();
    bool isRunning = true;
    CameraPosition* camPos = new CameraPosition(cPos,0,0, new Collider(Vec3{0,1.2,5}, 0.25), new Collider(Vec3{0,1.2,5}, 0.5));
    PlayerInfo pi = PlayerInfo(1, Vec3{0,-10,0},0,0);
    int scene = 0; 

    void setScene(int scn){
        scene = scn;
    }


    //----------------------------------------------------------------

    //-------------Asset Positions - Inside---------------

    Vec3 chairPos_01 = Vec3(-8, 0, 8.5f);
    Vec3 chairPos_02 = Vec3(-8, 0, 9.5f);
    Vec3 chairPos_03 = Vec3(-10, 0, 8.5f);
    Vec3 chairPos_04 = Vec3(-10, 0, 9.5f);

    Vec3 lampPos = Vec3(-1.875, 0.625f, -0.5);

    Vec3 radioPos = Vec3(-1.875, 0.625, 0.5);

    //Doors
    Vec3 mainDoorPos = Vec3(-0.05, 0, 15.25);
    Vec3 doorPos = Vec3(4.275f, 0.0f, 6.375f);
    Vec3 washroomDoorPos = Vec3(7.55f, 0.0f, 9.85f);

    Vec3 ghostPos = Vec3(-6, -5.75, 2);

    Vec3 spherePos = Vec3(0, -10, 0);

    Vec3 gamecubePos = Vec3(-13.375f, 0.4f, 0.0f);

    //Plants
    Vec3 plantPos_01 = Vec3(1.5f, 0.0f, 14.75f);
    Vec3 plantPos_02 = Vec3(3.25f, 0.0f, 15.0f);
    Vec3 plantPos_03 = Vec3(6.75f, 0.0f, 15.0f);
    Vec3 plantPos_04 = Vec3(8.5f, 0.0f, 15.0f);
    Vec3 plantPos_05 = Vec3(12.0f, 0.0f, 15.0f);
    Vec3 plantPos_06 = Vec3(12.0f, 0.0f, 7.5f);
    Vec3 plantPos_07 = Vec3(8.5f, 0.0f, 7.5f);
    Vec3 plantPos_08 = Vec3(6.75f, 0.0f, 7.5f);
    Vec3 plantPos_09 = Vec3(10.75f, 0.0f, 5.5f);
    Vec3 plantPos_10 = Vec3(10.75f, 0.0f, -4.0f);
    Vec3 plantPos_11 = Vec3(-0.25f, 0.0f, -4.0f);
    Vec3 plantPos_12 = Vec3(-0.25f, 0.0f, 5.5f);
    Vec3 plantPos_13 = Vec3(-2.0f, 0.0f, -4.25f);
    Vec3 plantPos_14 = Vec3(-13.5f, 0.0f, -4.25f);
    Vec3 plantPos_15 = Vec3(-13.5f, 0.0f, 14.25f);
    Vec3 plantPos_16 = Vec3(-0.5f, 0.0f, 14.75f);

    //Washing Machine
    Vec3 washingMachinePos_01 = Vec3(4.25f, 0.0f, 15.0f);
    Vec3 washingMachinePos_02 = Vec3(5.75f, 0.0f, 15.0f);

    //Washroom
    Vec3 bathtubPos = Vec3(10.3f, 0.6f, 15.0f);
    Vec3 toiletPos = Vec3(12.0f, 0.0f, 14.0f);
    Vec3 washroomCabinetPos_01 = Vec3(12.0f, 0.27f, 12.0f);
    Vec3 washroomSinkPos = Vec3(12.4f, 0.0f, 10.125f);
    Vec3 washroomCabinetPos_02 = Vec3(12.0f, 0.27f, 9.0f);
    Vec3 washroomCabinetPos_03 = Vec3(10.0f, 0.27f, 7.25f);

    //Kitchen
    Vec3 kitchenCabinetPos_01 = Vec3(-12.0f, 0.27f, 14.0f);
    Vec3 kitchenSinkPos = Vec3(-13.625f, 0.6f, 13.0f);
    Vec3 dishwasherPos = Vec3(-12.875f, 0.0f, 12.626f);
    Vec3 kitchenCabinetPos_02 = Vec3(-13.625f, 0.27f, 11.0f);
    Vec3 tablePos = Vec3(-9.0f, -0.2f, 9.0f);

    //Living Room
    Vec3 TVStandPos = Vec3(-13.625f, 0.0f, 0.0f);
    Vec3 sofaPos = Vec3(-8.0f, 0.0f, 0.0f);
    Vec3 smallTablePos = Vec3(-10.0f, 0.0f, 0.0f);
    Vec3 cabinetPos = Vec3(-1.875f, 0.27f, 0.0f);

    //Bedroom
    Vec3 bedPos = Vec3(5.0f, 0.0f, 0.0f);
    Vec3 nightstandPos_01 = Vec3(5.0f, 0.0f, -2.0f);
    Vec3 nightstandPos_02 = Vec3(5.0f, 0.0f, 2.0f);

    //Main Walls
    Vec3 floorPos = Vec3(0, 0, 0);
    Vec3 wallEastPos = Vec3(16.0f, 1.25f, 0);
    Vec3 wallWestPos = Vec3(-16.0f, 1.25f, 0);
    Vec3 wallNorthPos = Vec3(0, 1.25f, 16.0f);
    Vec3 wallSouthPos = Vec3(0, 1.25f, -16.0f);
    Vec3 ceilingPos = Vec3(0, 3.0f, 0);

    //Door Frames
    Vec3 doorFramePos_01 = Vec3(1.4, 0, 15.251);
    Vec3 doorFramePos_02 = Vec3(3.875f, 0.0f, 6.375f);
    Vec3 doorFramePos_03 = Vec3(5.65f, 0.0f, 6.38f);
    Vec3 doorFramePos_04 = Vec3(7.55f, 0.0f, 9.45f);
    Vec3 doorFramePos_05 = Vec3(7.555f, 0.0f, 11.225f);

    //Inner Walls
    Vec3 cornerPos_00 = Vec3(2.0f, 0.0f, 15.25f);
    Vec3 innerWallPos_01 = Vec3(2.0f, 0.0f, 12.0f);
    Vec3 cornerPos_01 = Vec3(2.0f, 0.0f, 11.625f);
    Vec3 cornerPos_02 = Vec3(2.75f, 0.0f, 11.625f);
    Vec3 innerWallPos_02 = Vec3(2.75f, 0.0f, 15.0f);
    Vec3 cornerPos_03 = Vec3(2.75f, 0.0f, 15.75f);
    Vec3 innerWallPos_03 = Vec3(6.5f, 0.0f, 15.75f);
    Vec3 cornerPos_04 = Vec3(7.25f, 0.0f, 15.75f);
    Vec3 innerWallPos_04 = Vec3(7.25f, 0.0f, 12.0f);
    Vec3 cornerPos_05 = Vec3(7.25f, 0.0f, 11.25f);
    Vec3 cornerPos_06 = Vec3(8.0f, 0.0f, 11.25f);
    Vec3 innerWallPos_05 = Vec3(8.0f, 0.0f, 15.0f);
    Vec3 cornerPos_07 = Vec3(8.0f, 0.0f, 15.75f);
    Vec3 innerWallPos_06 = Vec3(11.75f, 0.0f, 15.75f);
    Vec3 cornerPos_08 = Vec3(12.5f, 0.0f, 15.75f);
    Vec3 innerWallPos_07 = Vec3(12.5f, 0.0f, 12.0f);
    Vec3 innerWallPos_08 = Vec3(12.5f, 0.0f, 10.5f);
    Vec3 innerWallPos_09 = Vec3(12.5f, 0.0f, 7.5f);
    Vec3 cornerPos_09 = Vec3(12.5f, 0.0f, 6.75f);
    Vec3 innerWallPos_10 = Vec3(8.75f, 0.0f, 6.75f);
    Vec3 cornerPos_10 = Vec3(8.0f, 0.0f, 6.75f);
    Vec3 innerWallPos_11 = Vec3(8.0f, 0.0f, 9.0f);
    Vec3 cornerPos_11 = Vec3(8.0f, 0.0f, 9.375f);
    Vec3 cornerPos_12 = Vec3(7.25f, 0.0f, 9.375f);
    Vec3 innerWallPos_12 = Vec3(7.249f, 0.0f, 7.5f);
    Vec3 cornerPos_13 = Vec3(7.25f, 0.0f, 6.75f);
    Vec3 cornerPos_14 = Vec3(5.75f, 0.0f, 6.75f);
    Vec3 cornerPos_15 = Vec3(5.75f, 0.0f, 6.0f);
    Vec3 innerWallPos_13 = Vec3(7.625f, 0.0f, 5.999f);
    Vec3 innerWallPos_14 = Vec3(10.625f, 0.0f, 5.999f);
    Vec3 cornerPos_16 = Vec3(11.375f, 0.0f, 6.0f);
    Vec3 innerWallPos_15 = Vec3(11.375f, 0.0f, 2.25f);
    Vec3 innerWallPos_16 = Vec3(11.375f, 0.0f, -0.75f);
    Vec3 innerWallPos_17 = Vec3(11.375f, 0.0f, -3.75f);
    Vec3 cornerPos_17 = Vec3(11.375f, 0.0f, -4.5f);
    Vec3 innerWallPos_18 = Vec3(7.625f, 0.0f, -4.5f);
    Vec3 innerWallPos_19 = Vec3(4.625f, 0.0f, -4.5f);
    Vec3 innerWallPos_20 = Vec3(1.625f, 0.0f, -4.5f);
    Vec3 innerWallPos_21 = Vec3(0.125f, 0.0f, -4.5f);
    Vec3 cornerPos_18 = Vec3(-0.625f, 0.0f, -4.5f);
    Vec3 innerWallPos_22 = Vec3(-0.626f, 0.0f, -0.875f);
    Vec3 innerWallPos_23 = Vec3(-0.626f, 0.0f, 2.125f);
    Vec3 innerWallPos_24 = Vec3(-0.626f, 0.0f, 5.125f);
    Vec3 cornerPos_19 = Vec3(-0.625f, 0.0f, 5.875f);
    Vec3 innerWallPos_25 = Vec3(3.125f, 0.0f, 5.875f);
    Vec3 cornerPos_20 = Vec3(3.875f, 0.0f, 5.875f);
    Vec3 cornerPos_21 = Vec3(3.875f, 0.0f, 6.625f);
    Vec3 innerWallPos_26 = Vec3(2.0f, 0.0f, 6.626f);
    Vec3 innerWallPos_27 = Vec3(-1.0f, 0.0f, 6.626f);
    Vec3 cornerPos_22 = Vec3(-1.375f, 0.0f, 6.625f);
    Vec3 innerWallPos_28 = Vec3(-1.375f, 0.0f, 4.75f);
    Vec3 innerWallPos_29 = Vec3(-1.375f, 0.0f, 1.75f);
    Vec3 innerWallPos_30 = Vec3(-1.375f, 0.0f, -1.25f);
    Vec3 innerWallPos_31 = Vec3(-1.375f, 0.0f, -4.25f);
    Vec3 cornerPos_23 = Vec3(-1.375f, 0.0f, -5.0f);
    Vec3 innerWallPos_32 = Vec3(-4.375f, 0.0f, -4.999f);
    Vec3 innerWallPos_33 = Vec3(-7.375f, 0.0f, -4.999f);
    Vec3 innerWallPos_34 = Vec3(-10.375f, 0.0f, -4.999f);
    Vec3 innerWallPos_35 = Vec3(-13.375f, 0.0f, -4.999f);
    Vec3 cornerPos_24 = Vec3(-14.125f, 0.0f, -5.0f);
    Vec3 innerWallPos_36 = Vec3(-14.125f, 0.0f, -1.25f);
    Vec3 innerWallPos_37 = Vec3(-14.125f, 0.0f, 1.75f);
    Vec3 innerWallPos_38 = Vec3(-14.125f, 0.0f, 4.75f);
    Vec3 innerWallPos_39 = Vec3(-14.125f, 0.0f, 7.75f);
    Vec3 innerWallPos_40 = Vec3(-14.125f, 0.0f, 10.75f);
    Vec3 innerWallPos_41 = Vec3(-14.125f, 0.0f, 13.75f);
    Vec3 cornerPos_25 = Vec3(-14.125f, 0.0f, 14.5f);
    Vec3 innerWallPos_42 = Vec3(-10.375f, 0.0f, 14.5f);
    Vec3 innerWallPos_43 = Vec3(-7.375f, 0.0f, 14.5f);
    Vec3 innerWallPos_44 = Vec3(-4.375f, 0.0f, 14.5f);
    Vec3 innerWallPos_45 = Vec3(-1.375f, 0.0f, 14.5f);
    Vec3 cornerPos_26 = Vec3(-1.0f, 0.0f, 14.5f);
    Vec3 cornerPos_27 = Vec3(-1.0f, 0.0f, 15.25f);



    //-------------Colliders - Inside---------------

    Vec3 chairCol = Vec3(0.2f, 1.0f, 0.2f);

    float lampCol = 0.5f;

    Vec3 radioCol = Vec3(0.25f, 0.25f, 0.25f);

    Vec3 doorOffset = Vec3(0.5f, 0, 0);
    Vec3 doorCol = Vec3(0.75f, 1.0f, 0.25f);

    Vec3 washroomDoorOffset = Vec3(0, 0, 0.5f);
    Vec3 washroomDoorCol = Vec3(0.25f, 1.0f, 0.75f);

    float ghostCol = 0.4f;

    Vec3 gamecubeCol = Vec3(0.15f, 2.0f, 0.15f);
    
    Vec3 plantOffset = Vec3(0.0f, 1.2f, 0.0f);
    float plantCol = 0.4f;

    Vec3 washingMachineCol = Vec3(0.25f, 2.0f, 0.25f);

    //Washroom
    Vec3 bathtubCol = Vec3(1.0f, 2.0f, 0.5f);
    Vec3 toiletCol = Vec3(0.25f, 2.0f, 0.125f);
    Vec3 washroomCabinetCol_01 = Vec3(0.1f, 2.0f, 0.75f);
    Vec3 washroomCabinetCol_02 = Vec3(0.75f, 2.0f, 0.1f);
    Vec3 washroomSinkOffset = Vec3(0.0f, 2.0f, 0.6f);
    Vec3 washroomSinkCol = Vec3(0.6f, 2.0f, 0.6f);

    //Kitchen
    Vec3 kitchenSinkCol = Vec3(0.25f, 2.0f, 0.5f);
    Vec3 dishwasherOffset = Vec3(-0.75f, 0.0f, -0.25f);
    Vec3 dishwasherCol = Vec3(0.25f, 2.0f, 0.25f);


    //Main Walls
    Vec3 floorCol = Vec3(100.0f, 0.001f, 100.0f);
    Vec3 wallEastCol = Vec3(0.001f, 3.0f, 16.0f);
    Vec3 wallWestCol = Vec3(0.001f, 3.0f, 16.0f);
    Vec3 wallNorthCol = Vec3(16.0f, 3.0f, 0.001f);
    Vec3 wallSouthCol = Vec3(16.0f, 3.0f, 0.001f);
    Vec3 ceilingCol = Vec3();
    
    //Big Walls
    Vec3 innerWallOffset_01 = Vec3(1.5f, 0.0f, 0.05f);
    Vec3 innerWallCol_01 = Vec3(1.5f, 3.0f, 0.05f);

    Vec3 innerWallOffset_02 = Vec3(0.05f, 0.0f, -1.5f);
    Vec3 innerWallCol_02 = Vec3(0.05f, 3.0f, 1.5f);

    Vec3 innerWallOffset_03 = Vec3(-1.5f, 0.0f, -0.05f);
    Vec3 innerWallCol_03 = Vec3(1.5f, 3.0f, 0.05f);

    Vec3 innerWallOffset_04 = Vec3(-0.05f, 0.0f, 1.5f);
    Vec3 innerWallCol_04 = Vec3(0.05f, 3.0f, 1.5f);

    //Small Walls
    Vec3 innerWallOffset_05 = Vec3(0.75f, 0.0f, 0.05f);
    Vec3 innerWallCol_05 = Vec3(0.75f, 3.0f, 0.05f);

    Vec3 innerWallOffset_06 = Vec3(0.05f, 0.0f, -0.75f);
    Vec3 innerWallCol_06 = Vec3(0.05f, 3.0f, 0.75f);

    Vec3 innerWallOffset_07 = Vec3(-0.75f, 0.0f, -0.05f);
    Vec3 innerWallCol_07 = Vec3(0.75f, 3.0f, 0.05f);

    Vec3 innerWallOffset_08 = Vec3(-0.05f, 0.0f, 0.75f);
    Vec3 innerWallCol_08 = Vec3(0.05f, 3.0f, 0.75f);

    //Corners
    Vec3 cornerOffset_01 = Vec3(0.25f, 0.0f, -0.25f);
    Vec3 cornerOffset_02 = Vec3(-0.25f, 0.0f, -0.25f);
    Vec3 cornerOffset_03 = Vec3(-0.25f, 0.0f, 0.25f);
    Vec3 cornerOffset_04 = Vec3(0.25f, 0.0f, 0.25f);
    Vec3 cornerCol = Vec3(0.5f, 3.0f, 0.5f);

    //----------------------------------------------------------------
    
    //-------------Asset Positions - Outside---------------

    Vec3 housePos = Vec3(0, -0.5f, 0);

    Vec3 houseDoorPos = Vec3(0.9f, -0.27f, 1.3f);

    Vec3 outerChair1Pos = Vec3(-5.95f, -9.75f, 0.25f);
    Vec3 outerChair2Pos = Vec3(-4.35f, -9.75f, 1.3f);

    Vec3 outerWindowPos = Vec3(-2.625, -9.0f, -2.0f);

    Vec3 tree1_1Pos = Vec3(-5.45f, -8.8f, -2.375f);
    Vec3 tree1_2Pos = Vec3(-5.7f, -8.8f, -2.5625f);
    Vec3 tree1_3Pos = Vec3(-5.95f, -8.8f, -2.75f);

    Vec3 tree2_1Pos = Vec3(4.55f, -8.8f, -2.3f);
    Vec3 tree2_2Pos = Vec3(4.51875f, -8.8f, -2.45625f);
    Vec3 tree2_3Pos = Vec3(4.4875f, -8.8f, -2.6125f);
    Vec3 tree2_4Pos = Vec3(4.45625f, -8.8f, -2.76875f);
    Vec3 tree2_5Pos = Vec3(4.425f, -8.8f, -2.925f);
    Vec3 tree2_6Pos = Vec3(4.39375f, -8.8f, -3.08125f);
    Vec3 tree2_7Pos = Vec3(4.3625f, -8.8f, -3.2375f);
    Vec3 tree2_8Pos = Vec3(4.33125f, -8.8f, -3.39375f);
    Vec3 tree2_9Pos = Vec3(4.3f, -8.8f, -3.55f);

    Vec3 tree3_1Pos = Vec3(4.25f, -8.8f, 0.25f);
    Vec3 tree3_2Pos = Vec3(4.5f, -8.8f, 0.25f);
    Vec3 tree3_3Pos = Vec3(4.75f, -8.8f, 0.25f);

    Vec3 doorPost1Pos = Vec3(2.6f, -8.8f, 2.25f);
    Vec3 doorPost2Pos = Vec3(0.255f, -8.8f, 2.25f);

    //-------------Colliders - Outside---------------
    
    Vec3 invisibleOffset = Vec3(0, 10, 0);

    Vec3 houseCol = Vec3(1.25f, 1.5f, 1.25f);

    Vec3 houseDoorOffset = Vec3(0.5f, 0, 0);
    Vec3 houseDoorCol = Vec3(0.75f, 2.0f, 0.25f);

    Vec3 outerChair1Col = Vec3(0.5f, 2.0f, 1.0f);
    Vec3 outerChair2Col = Vec3(0.5f, 2.0f, 1.0f);

    Vec3 outerWindowCol = Vec3(1.0f, 1.0f, 0.5f);

    float tubeCol = 0.2f;

    std::array<Actor, 132> actors = {
        //Chair - 0
        Actor{0,-90, chairPos_01, Vec3{.025f,.025f,.025f}, "./meshes/Chair.obj", "./textures/chair.png", new Collider(chairPos_01 + chairCol,chairPos_01 - chairCol)},

        //Lamp - 1
        Actor{0,0, lampPos, Vec3{.01f,.01f,.01f}, "./meshes/Lamp.obj", "./textures/lamp.png", new Collider(lampPos, lampCol)},

        //Radio - 2
        Actor{0,-90, radioPos, Vec3{.0025f,.0025f,.0025f}, "./meshes/Radio.obj", "./textures/radio.png", new Collider(radioPos + radioCol,radioPos - radioCol)},

        //Room Door - 3
        Actor{0,0, doorPos, Vec3{.0082f,.0082f,.0082f}, "./meshes/Door.obj", "./textures/door.png", new Collider(doorPos + doorCol + doorOffset,doorPos - doorCol + doorOffset)},

        //MainDoor - 4
        Actor{0,0, mainDoorPos, Vec3{.009f,.009f,.009f}, "./meshes/Door.obj", "./textures/door.png", new Collider(mainDoorPos + doorCol + doorOffset,mainDoorPos - doorCol + doorOffset)},

        //Ghost - 5
        Actor{0,0, ghostPos, Vec3{.012f,.012f,.012f}, "./meshes/Ghost.obj", "./textures/ghost.png", new Collider(ghostPos, ghostCol)},
        
        //Sphere - 6
        Actor{0,0, spherePos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Sphere.obj", "./textures/ceiling.png"},

        //Gamecube - 7
        Actor{0,90, gamecubePos, Vec3{.0015f,.0015f,.0015f}, "./meshes/Gamecube.obj", "./textures/gamecube.png", new Collider(gamecubePos + gamecubeCol,gamecubePos - gamecubeCol)},

        //Washroom Door - 8
        Actor{0,0, washroomDoorPos, Vec3{.0082f,.0082f,.0082f}, "./meshes/Door.obj", "./textures/door.png"/*, new Collider(washroomDoorPos + washroomDoorCol + washroomDoorOffset,washroomDoorPos - washroomDoorCol + washroomDoorOffset)*/},

        //Chairs - 9, 10, 11
        Actor{0,-90, chairPos_02, Vec3{.025f,.025f,.025f}, "./meshes/Chair.obj", "./textures/chair.png", new Collider(chairPos_02 + chairCol,chairPos_02 - chairCol)},
        Actor{0,90, chairPos_03, Vec3{.025f,.025f,.025f}, "./meshes/Chair.obj", "./textures/chair.png", new Collider(chairPos_03 + chairCol,chairPos_03 - chairCol)},
        Actor{0,90, chairPos_04, Vec3{.025f,.025f,.025f}, "./meshes/Chair.obj", "./textures/chair.png", new Collider(chairPos_04 + chairCol,chairPos_04 - chairCol)},

        //Plants
        Actor{ 0,-90, plantPos_01, Vec3{0.05f,0.05f,0.05f}, "./meshes/Plant.obj", "./textures/plant.png", new Collider(plantPos_01 + plantOffset, plantCol)},
        Actor{ 0,-90, plantPos_02, Vec3{0.05f,0.05f,0.05f}, "./meshes/Plant.obj", "./textures/plant.png", new Collider(plantPos_02 + plantOffset, plantCol)},
        Actor{ 0,-90, plantPos_03, Vec3{0.05f,0.05f,0.05f}, "./meshes/Plant.obj", "./textures/plant.png", new Collider(plantPos_03 + plantOffset, plantCol)},
        Actor{ 0,-90, plantPos_04, Vec3{0.05f,0.05f,0.05f}, "./meshes/Plant.obj", "./textures/plant.png", new Collider(plantPos_04 + plantOffset, plantCol)},
        Actor{ 0,-90, plantPos_05, Vec3{0.05f,0.05f,0.05f}, "./meshes/Plant.obj", "./textures/plant.png", new Collider(plantPos_05 + plantOffset, plantCol)},
        Actor{ 0,-90, plantPos_06, Vec3{0.05f,0.05f,0.05f}, "./meshes/Plant.obj", "./textures/plant.png", new Collider(plantPos_06 + plantOffset, plantCol)},
        Actor{ 0,-90, plantPos_07, Vec3{0.05f,0.05f,0.05f}, "./meshes/Plant.obj", "./textures/plant.png", new Collider(plantPos_07 + plantOffset, plantCol)},
        Actor{ 0,-90, plantPos_08, Vec3{0.05f,0.05f,0.05f}, "./meshes/Plant.obj", "./textures/plant.png", new Collider(plantPos_08 + plantOffset, plantCol)},
        Actor{ 0,-90, plantPos_09, Vec3{0.05f,0.05f,0.05f}, "./meshes/Plant.obj", "./textures/plant.png", new Collider(plantPos_09 + plantOffset, plantCol)},
        Actor{ 0,-90, plantPos_10, Vec3{0.05f,0.05f,0.05f}, "./meshes/Plant.obj", "./textures/plant.png", new Collider(plantPos_10 + plantOffset, plantCol)},
        Actor{ 0,-90, plantPos_11, Vec3{0.05f,0.05f,0.05f}, "./meshes/Plant.obj", "./textures/plant.png", new Collider(plantPos_11 + plantOffset, plantCol)},
        Actor{ 0,-90, plantPos_12, Vec3{0.05f,0.05f,0.05f}, "./meshes/Plant.obj", "./textures/plant.png", new Collider(plantPos_12 + plantOffset, plantCol)},
        Actor{ 0,-90, plantPos_13, Vec3{0.05f,0.05f,0.05f}, "./meshes/Plant.obj", "./textures/plant.png", new Collider(plantPos_13 + plantOffset, plantCol)},
        Actor{ 0,-90, plantPos_14, Vec3{0.05f,0.05f,0.05f}, "./meshes/Plant.obj", "./textures/plant.png", new Collider(plantPos_14 + plantOffset, plantCol)},
        Actor{ 0,-90, plantPos_15, Vec3{0.05f,0.05f,0.05f}, "./meshes/Plant.obj", "./textures/plant.png", new Collider(plantPos_15 + plantOffset, plantCol)},
        Actor{ 0,-90, plantPos_16, Vec3{0.05f,0.05f,0.05f}, "./meshes/Plant.obj", "./textures/plant.png", new Collider(plantPos_16 + plantOffset, plantCol)},

        //Washing Machine
        Actor{ 0,0, washingMachinePos_01, Vec3{0.00125f,0.00125f,0.00125f}, "./meshes/Washing_Machine.obj", "./textures/washing_machine.png", new Collider(washingMachinePos_01 + washingMachineCol,washingMachinePos_01 - washingMachineCol)},
        Actor{ 0,0, washingMachinePos_02, Vec3{0.00125f,0.00125f,0.00125f}, "./meshes/Washing_Machine.obj", "./textures/washing_machine.png", new Collider(washingMachinePos_02 + washingMachineCol,washingMachinePos_02 - washingMachineCol)},

        //Washroom
        Actor{ 0,0, bathtubPos, Vec3{0.3f,0.3f,0.3f}, "./meshes/Bathtub.obj", "./textures/bathtub.png", new Collider(bathtubPos + bathtubCol,bathtubPos - bathtubCol)},
        Actor{ 0,-90, toiletPos, Vec3{1.0f,1.0f,1.0f}, "./meshes/Toilet.obj", "./textures/toilet.png", new Collider(toiletPos + toiletCol,toiletPos - toiletCol)},
        Actor{ 0,-90, washroomCabinetPos_01, Vec3{0.012f,0.012f,0.012f}, "./meshes/Cabinet.obj", "./textures/toilet.png", new Collider(washroomCabinetPos_01 + washroomCabinetCol_01,washroomCabinetPos_01 - washroomCabinetCol_01)},
        Actor{ 0,180, washroomSinkPos, Vec3{0.008f,0.008f,0.008f}, "./meshes/Bathroom_Sink.obj", "./textures/bathroom_sink.png", new Collider(washroomSinkPos + washroomSinkCol + washroomSinkOffset,washroomSinkPos - washroomSinkCol + washroomSinkOffset)},
        Actor{ 0,-90, washroomCabinetPos_02, Vec3{0.012f,0.012f,0.012f}, "./meshes/Cabinet.obj", "./textures/toilet.png", new Collider(washroomCabinetPos_02 + washroomCabinetCol_01,washroomCabinetPos_02 - washroomCabinetCol_01)},
        Actor{ 0,0, washroomCabinetPos_03, Vec3{0.012f,0.012f,0.012f}, "./meshes/Cabinet.obj", "./textures/toilet.png", new Collider(washroomCabinetPos_03 + washroomCabinetCol_02,washroomCabinetPos_03 - washroomCabinetCol_02)},

        //Kitchen
        Actor{ 0,180, kitchenCabinetPos_01, Vec3{0.012f,0.012f,0.012f}, "./meshes/Cabinet.obj", "./textures/toilet.png", new Collider(kitchenCabinetPos_01 + washroomCabinetCol_02,kitchenCabinetPos_01 - washroomCabinetCol_02)},
        Actor{ 0,90, kitchenSinkPos, Vec3{1.0f,1.0f,1.0f}, "./meshes/Kitchen_Sink.obj", "./textures/toilet.png", new Collider(kitchenSinkPos + kitchenSinkCol,kitchenSinkPos - kitchenSinkCol)},
        Actor{ 0,180, dishwasherPos, Vec3{0.001f,0.001f,0.001f}, "./meshes/Dishwasher.obj", "./textures/toilet.png", new Collider(dishwasherPos + kitchenSinkCol + dishwasherOffset,dishwasherPos - kitchenSinkCol + dishwasherOffset)},
        Actor{ 0,90, kitchenCabinetPos_02, Vec3{0.012f,0.012f,0.012f}, "./meshes/Cabinet.obj", "./textures/toilet.png", new Collider(kitchenCabinetPos_02 + washroomCabinetCol_01,kitchenCabinetPos_02 - washroomCabinetCol_01)},
        Actor{ 0,90, tablePos, Vec3{0.012f,0.012f,0.012f}, "./meshes/Table.obj", "./textures/wood.png", new Collider(tablePos + washroomCabinetCol_01,tablePos - washroomCabinetCol_01)},

        //Living Room
        Actor{ 0,-90, TVStandPos, Vec3{0.009f,0.009f,0.009f}, "./meshes/TVStand.obj", "./textures/TVStand.png", new Collider(TVStandPos + washroomCabinetCol_01,TVStandPos - washroomCabinetCol_01)},
        Actor{ 0,-90, sofaPos, Vec3{0.005f,0.005f,0.005f}, "./meshes/Sofa.obj", "./textures/sofa.png", new Collider(sofaPos + washroomCabinetCol_01,sofaPos - washroomCabinetCol_01)},
        Actor{ 0,90, smallTablePos, Vec3{0.009f,0.009f,0.009f}, "./meshes/Table.obj", "./textures/wood.png", new Collider(smallTablePos + washroomCabinetCol_01,smallTablePos - washroomCabinetCol_01)},
        Actor{ 0,-90, cabinetPos, Vec3{0.012f,0.012f,0.012f}, "./meshes/Cabinet.obj", "./textures/chair.png", new Collider(cabinetPos + washroomCabinetCol_01,cabinetPos - washroomCabinetCol_01)},

        //Bedroom
        Actor{ 0,180, bedPos, Vec3{0.0025f,0.0025f,0.0025f}, "./meshes/Bed.obj", "./textures/wood.png", new Collider(bedPos + washroomCabinetCol_01,bedPos - washroomCabinetCol_01)},
        Actor{ 0,-90, nightstandPos_01, Vec3{0.009f,0.009f,0.009f}, "./meshes/Night_Stand.obj", "./textures/wood.png", new Collider(nightstandPos_01 + washroomCabinetCol_01,nightstandPos_01 - washroomCabinetCol_01)},
        Actor{ 0,-90, nightstandPos_02, Vec3{0.009f,0.009f,0.009f}, "./meshes/Night_Stand.obj", "./textures/wood.png", new Collider(nightstandPos_02 + washroomCabinetCol_01,nightstandPos_02 - washroomCabinetCol_01)},

        //Floor
        Actor{0,0, floorPos, Vec3{100.0f,0.001f,100.0f}, "./meshes/cube.obj", "./textures/wood.png", new Collider(floorPos + floorCol,floorPos - floorCol)},

        //Wall 1
        Actor{0,0, wallEastPos, Vec3{0.001f,3.0f,16.0f}, "./meshes/cube.obj", "./textures/wood.png", new Collider(wallEastPos + wallEastCol,wallEastPos - wallEastCol)},

        //Wall 2
        Actor{0,0, wallWestPos, Vec3{0.001f,3.0f,16.0f}, "./meshes/cube.obj", "./textures/wood.png", new Collider(wallWestPos + wallWestCol,wallWestPos - wallWestCol)},

        //Wall 3
        Actor{0,0, wallNorthPos, Vec3{16.0f,3.0f,0.001f}, "./meshes/cube.obj", "./textures/wood.png", new Collider(wallNorthPos + wallNorthCol,wallNorthPos - wallNorthCol)},

        //Wall 4
        Actor{0,0, wallSouthPos, Vec3{16.0f,3.0f,0.001f}, "./meshes/cube.obj", "./textures/wood.png", new Collider(wallSouthPos + wallSouthCol,wallSouthPos - wallSouthCol)},

        //Ceiling
        Actor{0,0, ceilingPos, Vec3{16.0f,0.001f,16.0f}, "./meshes/cube.obj", "./textures/ceiling.png"},

        //Door Frames
        Actor{0,180, doorFramePos_01, Vec3{0.009f,0.0075f,0.0075f}, "./meshes/Door_Frame.obj", "./textures/wall.png"},
        Actor{0,0, doorFramePos_02, Vec3{0.0089f,0.0075f,0.0075f}, "./meshes/Door_Frame.obj", "./textures/wall.png"},
        Actor{0,180, doorFramePos_03, Vec3{0.0089f,0.0075f,0.0075f}, "./meshes/Door_Frame.obj", "./textures/wall.png"},
        Actor{0,-90, doorFramePos_04, Vec3{0.0089f,0.0075f,0.0075f}, "./meshes/Door_Frame.obj", "./textures/wall.png"},
        Actor{0,90, doorFramePos_05, Vec3{0.0089f,0.0075f,0.0075f}, "./meshes/Door_Frame.obj", "./textures/wall.png"},

        //Inner Walls
        Actor{ 0,180, cornerPos_00, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_02.obj", "./textures/wall.png" },

        Actor{0,-90, innerWallPos_01, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_01 + innerWallCol_04 + innerWallOffset_04,innerWallPos_01 - innerWallCol_04 + innerWallOffset_04)},
        
        Actor{0,-90, cornerPos_01, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_01.obj", "./textures/wall.png", new Collider(cornerPos_01 + cornerCol + cornerOffset_04,cornerPos_01 - cornerCol + cornerOffset_04)},
        Actor{0,180, cornerPos_02, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_01.obj", "./textures/wall.png", new Collider(cornerPos_02 + cornerCol + cornerOffset_03,cornerPos_02 - cornerCol + cornerOffset_03)},

        Actor{0,90, innerWallPos_02, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_02 + innerWallCol_02 + innerWallOffset_02,innerWallPos_02 - innerWallCol_02 + innerWallOffset_02)},
        
        Actor{0,90, cornerPos_03, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_02.obj", "./textures/wall.png"},

        Actor{0,180, innerWallPos_03, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_03 + innerWallCol_03 + innerWallOffset_03,innerWallPos_03 - innerWallCol_03 + innerWallOffset_03)},

        Actor{0,180, cornerPos_04, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_02.obj", "./textures/wall.png"},

        Actor{0,-90, innerWallPos_04, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_04 + innerWallCol_04 + innerWallOffset_04,innerWallPos_04 - innerWallCol_04 + innerWallOffset_04)},

        Actor{0,-90, cornerPos_05, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_01.obj", "./textures/wall.png", new Collider(cornerPos_05 + cornerCol + cornerOffset_04,cornerPos_05 - cornerCol + cornerOffset_04)},
        Actor{0,180, cornerPos_06, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_01.obj", "./textures/wall.png", new Collider(cornerPos_06 + cornerCol + cornerOffset_03,cornerPos_06 - cornerCol + cornerOffset_03)},

        Actor{0,90, innerWallPos_05, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_05 + innerWallCol_02 + innerWallOffset_02,innerWallPos_05 - innerWallCol_02 + innerWallOffset_02)},

        Actor{0,90, cornerPos_07, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_02.obj", "./textures/wall.png"},

        Actor{0,180, innerWallPos_06, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_06 + innerWallCol_03 + innerWallOffset_03,innerWallPos_06 - innerWallCol_03 + innerWallOffset_03)},

        Actor{0,180, cornerPos_08, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_02.obj", "./textures/wall.png"},
        
        Actor{0,-90, innerWallPos_07, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_07 + innerWallCol_04 + innerWallOffset_04,innerWallPos_07 - innerWallCol_04 + innerWallOffset_04)},
        Actor{0,-90, innerWallPos_08, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_01.obj", "./textures/wall.png", new Collider(innerWallPos_08 + innerWallCol_08 + innerWallOffset_08,innerWallPos_08 - innerWallCol_08 + innerWallOffset_08)},
        Actor{0,-90, innerWallPos_09, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_09 + innerWallCol_04 + innerWallOffset_04,innerWallPos_09 - innerWallCol_04 + innerWallOffset_04)},
        
        Actor{0,-90, cornerPos_09, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_02.obj", "./textures/wall.png"},

        Actor{0,0, innerWallPos_10, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_10 + innerWallCol_01 + innerWallOffset_01,innerWallPos_10 - innerWallCol_01 + innerWallOffset_01)},

        Actor{0,0, cornerPos_10, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_02.obj", "./textures/wall.png"},
        
        Actor{0,90, innerWallPos_11, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_01.obj", "./textures/wall.png", new Collider(innerWallPos_11 + innerWallCol_06 + innerWallOffset_06,innerWallPos_11 - innerWallCol_06 + innerWallOffset_06)},

        Actor{0,90, cornerPos_11, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_01.obj", "./textures/wall.png", new Collider(cornerPos_11 + cornerCol + cornerOffset_02,cornerPos_11 - cornerCol + cornerOffset_02)},
        Actor{0,0, cornerPos_12, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_01.obj", "./textures/wall.png", new Collider(cornerPos_12 + cornerCol + cornerOffset_01,cornerPos_12 - cornerCol + cornerOffset_01)},

        Actor{0,-90, innerWallPos_12, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_01.obj", "./textures/wall.png", new Collider(innerWallPos_12 + innerWallCol_08 + innerWallOffset_08,innerWallPos_12 - innerWallCol_08 + innerWallOffset_08)},

        Actor{0,-90, cornerPos_13, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_02.obj", "./textures/wall.png"},
        Actor{0,0, cornerPos_14, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_01.obj", "./textures/wall.png", new Collider(cornerPos_14 + cornerCol + cornerOffset_01,cornerPos_14 - cornerCol + cornerOffset_01)},
        Actor{0,-90, cornerPos_15, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_01.obj", "./textures/wall.png", new Collider(cornerPos_15 + cornerCol + cornerOffset_04,cornerPos_15 - cornerCol + cornerOffset_04)},

        Actor{0,180, innerWallPos_13, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_01.obj", "./textures/wall.png", new Collider(innerWallPos_13 + innerWallCol_07 + innerWallOffset_07,innerWallPos_13 - innerWallCol_07 + innerWallOffset_07)},
        Actor{0,180, innerWallPos_14, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_14 + innerWallCol_03 + innerWallOffset_03,innerWallPos_14 - innerWallCol_03 + innerWallOffset_03)},

        Actor{0,180, cornerPos_16, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_02.obj", "./textures/wall.png"},

        Actor{0,-90, innerWallPos_15, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_15 + innerWallCol_04 + innerWallOffset_04,innerWallPos_15 - innerWallCol_04 + innerWallOffset_04)},
        Actor{0,-90, innerWallPos_16, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_16 + innerWallCol_04 + innerWallOffset_04,innerWallPos_16 - innerWallCol_04 + innerWallOffset_04)},
        Actor{0,-90, innerWallPos_17, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_17 + innerWallCol_04 + innerWallOffset_04,innerWallPos_17 - innerWallCol_04 + innerWallOffset_04)},

        Actor{0,-90, cornerPos_17, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_02.obj", "./textures/wall.png"},

        Actor{ 0,0, innerWallPos_18, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_18 + innerWallCol_01 + innerWallOffset_01,innerWallPos_18 - innerWallCol_01 + innerWallOffset_01) },
        Actor{ 0,0, innerWallPos_19, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_19 + innerWallCol_01 + innerWallOffset_01,innerWallPos_19 - innerWallCol_01 + innerWallOffset_01) },
        Actor{ 0,0, innerWallPos_20, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_20 + innerWallCol_01 + innerWallOffset_01,innerWallPos_20 - innerWallCol_01 + innerWallOffset_01) },
        Actor{ 0,0, innerWallPos_21, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_01.obj", "./textures/wall.png", new Collider(innerWallPos_21 + innerWallCol_05 + innerWallOffset_05,innerWallPos_21 - innerWallCol_05 + innerWallOffset_05) },

        Actor{ 0,0, cornerPos_18, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_02.obj", "./textures/wall.png"},

        Actor{ 0,90, innerWallPos_22, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_22 + innerWallCol_02 + innerWallOffset_02,innerWallPos_22 - innerWallCol_02 + innerWallOffset_02) },
        Actor{ 0,90, innerWallPos_23, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_23 + innerWallCol_02 + innerWallOffset_02,innerWallPos_23 - innerWallCol_02 + innerWallOffset_02) },
        Actor{ 0,90, innerWallPos_24, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_24 + innerWallCol_02 + innerWallOffset_02,innerWallPos_24 - innerWallCol_02 + innerWallOffset_02) },

        Actor{ 0,90, cornerPos_19, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_02.obj", "./textures/wall.png"},
        
        Actor{ 0,180, innerWallPos_25, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_25 + innerWallCol_03 + innerWallOffset_03,innerWallPos_25 - innerWallCol_03 + innerWallOffset_03) },
        
        Actor{ 0,180, cornerPos_20, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_01.obj", "./textures/wall.png", new Collider(cornerPos_20 + cornerCol + cornerOffset_03,cornerPos_20 - cornerCol + cornerOffset_03) },
        Actor{ 0,90, cornerPos_21, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_01.obj", "./textures/wall.png", new Collider(cornerPos_21 + cornerCol + cornerOffset_02,cornerPos_21 - cornerCol + cornerOffset_02) },

        Actor{ 0,0, innerWallPos_26, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_01.obj", "./textures/wall.png", new Collider(innerWallPos_26 + innerWallCol_05 + innerWallOffset_05,innerWallPos_26 - innerWallCol_05 + innerWallOffset_05) },
        Actor{ 0,0, innerWallPos_27, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_27 + innerWallCol_01 + innerWallOffset_01,innerWallPos_27 - innerWallCol_01 + innerWallOffset_01) },

        Actor{ 0,0, cornerPos_22, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_01.obj", "./textures/wall.png", new Collider(cornerPos_22 + cornerCol + cornerOffset_01,cornerPos_22 - cornerCol + cornerOffset_01) },

        Actor{ 0,-90, innerWallPos_28, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_01.obj", "./textures/wall.png", new Collider(innerWallPos_28 + innerWallCol_08 + innerWallOffset_08,innerWallPos_28 - innerWallCol_08 + innerWallOffset_08) },
        Actor{ 0,-90, innerWallPos_29, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_29 + innerWallCol_04 + innerWallOffset_04,innerWallPos_29 - innerWallCol_04 + innerWallOffset_04) },
        Actor{ 0,-90, innerWallPos_30, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_30 + innerWallCol_04 + innerWallOffset_04,innerWallPos_30 - innerWallCol_04 + innerWallOffset_04) },
        Actor{ 0,-90, innerWallPos_31, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_31 + innerWallCol_04 + innerWallOffset_04,innerWallPos_31 - innerWallCol_04 + innerWallOffset_04) },

        Actor{ 0,-90, cornerPos_23, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_02.obj", "./textures/wall.png" },
        
        Actor{ 0,0, innerWallPos_32, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_32 + innerWallCol_01 + innerWallOffset_01,innerWallPos_32 - innerWallCol_01 + innerWallOffset_01) },
        Actor{ 0,0, innerWallPos_33, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_33 + innerWallCol_01 + innerWallOffset_01,innerWallPos_33 - innerWallCol_01 + innerWallOffset_01) },
        Actor{ 0,0, innerWallPos_34, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_34 + innerWallCol_01 + innerWallOffset_01,innerWallPos_34 - innerWallCol_01 + innerWallOffset_01) },
        Actor{ 0,0, innerWallPos_35, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_35 + innerWallCol_01 + innerWallOffset_01,innerWallPos_35 - innerWallCol_01 + innerWallOffset_01) },

        Actor{ 0,0, cornerPos_24, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_02.obj", "./textures/wall.png" },
           
        Actor{ 0,90, innerWallPos_36, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_36 + innerWallCol_02 + innerWallOffset_02,innerWallPos_36 - innerWallCol_02 + innerWallOffset_02) },
        Actor{ 0,90, innerWallPos_37, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_37 + innerWallCol_02 + innerWallOffset_02,innerWallPos_37 - innerWallCol_02 + innerWallOffset_02) },
        Actor{ 0,90, innerWallPos_38, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_38 + innerWallCol_02 + innerWallOffset_02,innerWallPos_38 - innerWallCol_02 + innerWallOffset_02) },
        Actor{ 0,90, innerWallPos_39, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_39 + innerWallCol_02 + innerWallOffset_02,innerWallPos_39 - innerWallCol_02 + innerWallOffset_02) },
        Actor{ 0,90, innerWallPos_40, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_40 + innerWallCol_02 + innerWallOffset_02,innerWallPos_40 - innerWallCol_02 + innerWallOffset_02) },
        Actor{ 0,90, innerWallPos_41, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_41 + innerWallCol_02 + innerWallOffset_02,innerWallPos_41 - innerWallCol_02 + innerWallOffset_02) },

        Actor{ 0,90, cornerPos_25, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_02.obj", "./textures/wall.png" },

        Actor{ 0,180, innerWallPos_42, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_42 + innerWallCol_03 + innerWallOffset_03,innerWallPos_42 - innerWallCol_03 + innerWallOffset_03) },
        Actor{ 0,180, innerWallPos_43, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_43 + innerWallCol_03 + innerWallOffset_03,innerWallPos_43 - innerWallCol_03 + innerWallOffset_03) },
        Actor{ 0,180, innerWallPos_44, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_44 + innerWallCol_03 + innerWallOffset_03,innerWallPos_44 - innerWallCol_03 + innerWallOffset_03) },
        Actor{ 0,180, innerWallPos_45, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_02.obj", "./textures/wall.png", new Collider(innerWallPos_45 + innerWallCol_03 + innerWallOffset_03,innerWallPos_45 - innerWallCol_03 + innerWallOffset_03) },

        Actor{ 0,180, cornerPos_26, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_01.obj", "./textures/wall.png", new Collider(cornerPos_26 + cornerCol + cornerOffset_03,cornerPos_26 - cornerCol + cornerOffset_03) },
        Actor{ 0,90, cornerPos_27, Vec3{0.0075f,0.0075f,0.0075f}, "./meshes/Wall_Corner_02.obj", "./textures/wall.png" },
    };

    std::array<Actor, 22> actors2 = {

        //House - 0
        Actor{0,0, housePos, Vec3{3.0f,3.0f,3.0f}, "./meshes/Cube.obj", "./textures/wood.png", new Collider(Vec3(3.3f, 4.3f, 1.3f), Vec3(-3.3f, -2.3f, -5.3f))},

        //House Door - 1
        Actor{0,0, houseDoorPos, Vec3{.009f,.01f,.009f}, "./meshes/Door.obj", "./textures/door.png", new Collider(houseDoorPos + houseDoorCol + houseDoorOffset,houseDoorPos - houseDoorCol + houseDoorOffset)},

        //Chair 1 - 2
        Actor{0,6, outerChair1Pos, Vec3{0.5f,0.6f,1.0f}, "./meshes/Cube.obj", "./textures/door.png", new Collider(outerChair1Pos + outerChair1Col + invisibleOffset,outerChair1Pos - outerChair1Col + invisibleOffset)},

        //Chair 2 - 3
        Actor{0,30, outerChair2Pos, Vec3{0.5f,0.6f,1.0f}, "./meshes/Cube.obj", "./textures/door.png", new Collider(outerChair2Pos + outerChair2Col + invisibleOffset,outerChair2Pos - outerChair2Col + invisibleOffset)},

        //Window
        Actor{0,0, outerWindowPos, Vec3{1.0f,1.0f,0.5f}, "./meshes/Cube.obj", "./textures/door.png", new Collider(outerWindowPos + outerWindowCol + invisibleOffset,outerWindowPos - outerWindowCol + invisibleOffset)},

        //Tree 1
        Actor{0,0, tree1_1Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(tree1_1Pos + invisibleOffset,tubeCol)},
        Actor{0,0, tree1_2Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(tree1_2Pos + invisibleOffset,tubeCol)},
        Actor{0,0, tree1_3Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(tree1_3Pos + invisibleOffset,tubeCol)},

        //Tree 2
        Actor{0,0, tree2_1Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(tree2_1Pos + invisibleOffset,tubeCol)},
        Actor{0,0, tree2_2Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(tree2_2Pos + invisibleOffset,tubeCol)},
        Actor{0,0, tree2_3Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(tree2_3Pos + invisibleOffset,tubeCol)},
        Actor{0,0, tree2_4Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(tree2_4Pos + invisibleOffset,tubeCol)},
        Actor{0,0, tree2_5Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(tree2_5Pos + invisibleOffset,tubeCol)},
        Actor{0,0, tree2_6Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(tree2_6Pos + invisibleOffset,tubeCol)},
        Actor{0,0, tree2_7Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(tree2_7Pos + invisibleOffset,tubeCol)},
        Actor{0,0, tree2_8Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(tree2_8Pos + invisibleOffset,tubeCol)},
        Actor{0,0, tree2_9Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(tree2_9Pos + invisibleOffset,tubeCol)},

        //Tree 3
        Actor{0,0, tree3_1Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(tree3_1Pos + invisibleOffset,tubeCol)},
        Actor{0,0, tree3_2Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(tree3_2Pos + invisibleOffset,tubeCol)},
        Actor{0,0, tree3_3Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(tree3_3Pos + invisibleOffset,tubeCol)},

        //Door Post 1
        Actor{0,0, doorPost1Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(doorPost1Pos + invisibleOffset,tubeCol)},

        //Door Post 2
        Actor{0,0, doorPost2Pos, Vec3{0.25f,0.25f,0.25f}, "./meshes/Cube.obj", "./textures/chair.png", new Collider(doorPost2Pos + invisibleOffset,tubeCol)},

    };
        
private:
    const size_t MAX_FRAMES_IN_FLIGHT = 2;
    const static int textureCount{ 2 };
    static const int mariosToPaint = 1;
    float fframe = 0;

    SDL_Event sdlEvent;
    uint32_t windowWidth;
    uint32_t windowHeight;
    SDL_Window* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipelineID;

    int descriptorSetsArrayIndex = 0;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    VkCommandPool commandPool;
    IndexedBufferMemory indexedBufferMemory;
    IndexedBufferMemory indexedBufferMemorySphere;


    // std::vector<BufferMemory> cameraBuffers;
    std::vector<VkBuffer> normalsBuffers;
    std::vector<VkDeviceMemory> normalsBuffersMemory;
    std::vector<VkBuffer> cameraBuffers;
    std::vector<VkDeviceMemory> cameraBuffersMemory;
    //std::vector<BufferMemory> globalLightingBuffers;
    std::vector<VkBuffer> glightingBuffers;
    std::vector<VkDeviceMemory> glightingBuffersMemory;

    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;

    bool framebufferResized = false;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;


    /// <summary>
    /// /////////
    /// </summary>

    CameraUBO cameraUBO;
    GlobalLighting glightsUBO;
    NormalUBO normalUBO{1, Vec4{255,255,255,1}};

    bool hasStencilComponent(VkFormat format);
    void initVulkan();
    void createInstance();
    void createSurface();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void recreateSwapChain();
    void updateCameraUniformBuffer(uint32_t currentImage);
    void updateGLightsUniformBuffer(uint32_t currentImage);
    void updateNormalsUniformBuffer(uint32_t currentImage);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void createRenderPass();
    void createDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout);
    void createGraphicsPipeline(const char* vFilename, const char* fFilename, VkPipeline& pipeline, VkPipelineLayout& pipelineLayout);
    void createFramebuffers();
    void createCommandPool();
    void createDepthResources();
    void createTextureImage(const char* filename, VkImage& textureImage);
    void createTextureImageView(VkImageView& textureImageView, VkImage textureImage);
    void createTextureSampler(VkSampler& textureSampler);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void loadModel(const char* filename, IndexedBufferMemory& indexedBufferMemory);
    void createVertexBuffer(IndexedBufferMemory& indexedBufferMemory, const std::vector<Vertex>& vertices);
    /// A helper function for createVertexBuffer()
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createIndexBuffer(IndexedBufferMemory& indexedBufferMemory, const std::vector<uint32_t>& indices);
    void createUniformBuffers(VkDeviceSize bufferSize, std::vector<VkBuffer>& uniformBuffer, std::vector<VkDeviceMemory>& uniformBufferMemory);
    void destroyUniformBuffer(std::vector<VkBuffer>& uniformBuffer, std::vector<VkDeviceMemory>& uniformBufferMemory);
    void createDescriptorPool(VkDescriptorPool& descriptorPool);
    void createDescriptorSets(VkDescriptorSetLayout& descriptorSetLayout, std::vector<VkDescriptorSet>& descriptorSets, VkDescriptorPool descriptorPool, VkImageView& textureImageView, VkSampler& textureSample);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void createCommandBuffers();
    void actorLoad(Actor* actor);

    void recordCommandBuffer();
    void createSyncObjects();
    void cleanup();
    void cleanupActor(Actor* actor);
    void cleanupSwapChain();
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    VkFormat findDepthFormat();
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    void  populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void setupDebugMessenger();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    VkDeviceMemory textureImageMemory;

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    VkShaderModule createShaderModule(const std::vector<char>& code);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    static std::vector<char> readFile(const std::string& filename);


};
#endif 

