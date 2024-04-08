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
using namespace MATH;


#include "Renderer.h"


const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG /// only use validation layers if in debug mode
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
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

    int scene = 0;

    void setScene(int scn){
        scene = scn;
    }
    //Asset Positions - Inside

    Vec3 chairPos = Vec3(-2, 0, 0);
    Vec3 lampPos = Vec3(0, 0.75f, 0);
    Vec3 radioPos = Vec3(2, 0.75, 0);
    Vec3 doorPos = Vec3(-4, 0, 0);
    Vec3 mainDoorPos = Vec3(0, 0, 14.9);
    Vec3 roombaPos = Vec3(2, 0.75, 4);
    Vec3 floorPos = Vec3(0, 0, 0);
    Vec3 wallEastPos = Vec3(15, 1.25f, 0);
    Vec3 wallWestPos = Vec3(-15, 1.25f, 0);
    Vec3 wallNorthPos = Vec3(0, 1.25f, 15);
    Vec3 wallSouthPos = Vec3(0, 1.25f, -15);
    Vec3 ceilingPos = Vec3(0, 4.25f, 0);
    
    //Colliders - Inside

    Vec3 chairCol = Vec3(0.2f, 1.0f, 0.2f);
    float lampCol = 0.4f;
    Vec3 radioCol = Vec3(0.25f, 0.25f, 0.25f);
    Vec3 doorCol = Vec3(0.75f, 1.0f, 0.25f);
    Vec3 doorOffset = Vec3(0.5f, 0, 0);
    float roombaCol = 0.4f;
    Vec3 floorCol = Vec3(100.0f, 0.001f, 100.0f);
    Vec3 wallEastCol = Vec3(0.001f, 3.0f, 15.0f);
    Vec3 wallWestCol = Vec3(0.001f, 3.0f, 15.0f);
    Vec3 wallNorthCol = Vec3(15.0f, 3.0f, 0.001f);
    Vec3 wallSouthCol = Vec3(15.0f, 3.0f, 0.001f);
    //Vec3 ceilingCol = Vec3();

    //----------------------------------------------------------------
    
    //Asset Positions - Outside

    Vec3 housePos = Vec3(0, -0.5f, 0);
    Vec3 houseDoorPos = Vec3(0.9f, -0.1f, 1.25f);
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

    //Colliders - Outside
    
    Vec3 invisibleOffset = Vec3(0, 10, 0);
    Vec3 houseCol = Vec3(1.25f, 1.5f, 1.25f);
    Vec3 houseDoorCol = Vec3(0.75f, 2.0f, 0.25f);
    Vec3 houseDoorOffset = Vec3(0.5f, 0, 0);
    Vec3 outerChair1Col = Vec3(0.5f, 2.0f, 1.0f);
    Vec3 outerChair2Col = Vec3(0.5f, 2.0f, 1.0f);
    Vec3 outerWindowCol = Vec3(1.0f, 1.0f, 0.5f);
    float tubeCol = 0.2f;

    std::array<Actor, 12> actors = {
        //Chair - 0
        Actor{0,0, chairPos, Vec3{.025f,.025f,.025f}, "./meshes/Chair.obj", "./textures/chair.png", new Collider(chairPos + chairCol,chairPos - chairCol)},

        //Lamp - 1
        Actor{0,0, lampPos, Vec3{.01f,.01f,.01f}, "./meshes/Lamp.obj", "./textures/chair.png", new Collider(lampPos, lampCol)},

        //Radio - 2
        Actor{0,0, radioPos, Vec3{.0025f,.0025f,.0025f}, "./meshes/Radio.obj", "./textures/chair.png", new Collider(radioPos + radioCol,radioPos - radioCol)},

        //Door - 3
        Actor{0,0, doorPos, Vec3{.009f,.009f,.009f}, "./meshes/Door.obj", "./textures/door.png", new Collider(doorPos + doorCol + doorOffset,doorPos - doorCol + doorOffset)},

        //MainDoor - 4
        Actor{0,0, mainDoorPos, Vec3{.009f,.009f,.009f}, "./meshes/Door.obj", "./textures/door.png", new Collider(mainDoorPos + doorCol + doorOffset,mainDoorPos - doorCol + doorOffset)},

        //Roomba - 4
        Actor{0,0, roombaPos, Vec3{.025f,.025f,.025f}, "./meshes/Roomba.obj", "./textures/door.png", new Collider(roombaPos, roombaCol)},

        //Floor
        Actor{0,0, floorPos, Vec3{100.0f,0.001f,100.0f}, "./meshes/cube.obj", "./textures/door.png", new Collider(floorPos + floorCol,floorPos - floorCol)},

        //Wall 1
        Actor{0,0, wallEastPos, Vec3{0.001f,3.0f,15.0f}, "./meshes/cube.obj", "./textures/door.png", new Collider(wallEastPos + wallEastCol,wallEastPos - wallEastCol)},

        //Wall 2
        Actor{0,0, wallWestPos, Vec3{0.001f,3.0f,15.0f}, "./meshes/cube.obj", "./textures/door.png", new Collider(wallWestPos + wallWestCol,wallWestPos - wallWestCol)},

        //Wall 3
        Actor{0,0, wallNorthPos, Vec3{15.0f,3.0f,0.001f}, "./meshes/cube.obj", "./textures/door.png", new Collider(wallNorthPos + wallNorthCol,wallNorthPos - wallNorthCol)},

        //Wall 4
        Actor{0,0, wallSouthPos, Vec3{15.0f,3.0f,0.001f}, "./meshes/cube.obj", "./textures/door.png", new Collider(wallSouthPos + wallSouthCol,wallSouthPos - wallSouthCol)},

        //Ceiling
        Actor{0,0, ceilingPos, Vec3{15.0f,0.001f,15.0f}, "./meshes/cube.obj", "./textures/door.png"},

        //ColliderBox
        //Actor{0,0, Vec3{-2,0,0}, Vec3{.4f,1.2f,.4f}, "./meshes/cube.obj", "./textures/door.png"}
        // -2.4, -2.45, -0.4       -1.6, -0.05, 0.4
    };

    std::array<Actor, 22> actors2 = {

        //House - 0
        Actor{0,0, housePos, Vec3{3.0f,3.0f,3.0f}, "./meshes/House.obj", "./textures/door.png", new Collider(Vec3(3.3f, 4.3f, 1.3f), Vec3(-3.3f, -2.3f, -5.3f))},

        //House Door - 1
        Actor{0,0, houseDoorPos, Vec3{.009f,.009f,.009f}, "./meshes/Door.obj", "./textures/door.png", new Collider(houseDoorPos + houseDoorCol + houseDoorOffset,houseDoorPos - houseDoorCol + houseDoorOffset)},

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

