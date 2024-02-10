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
    Vec4  lightPos;
};

struct NormalUBO {
    float plength;
    Vec4 colour;
};

/// Global Lighting for the UBO is in a separate header file
/// GlobalLighting.h

/// NEW
struct ModelMatrixPushConst {
    Matrix4 modelMatrix;
    Matrix4 normalMatrix;
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
    void SetCameraUBO(const Matrix4& projection_, const Matrix4& view_);
    void SetGLightsUBO(const GlobalLighting& glightsUBO_);
    void SetNormalUBO(const NormalUBO& normalUBO_);
    void SetSphereModelMatrixPush(const Matrix4& modelMatrix);
    void SetModelMatrixPush(const Matrix4& modelMatrix);
    void SetTextureIndex(int textureIndex);
    SDL_Window* GetWindow() { return window; }


private:
    const size_t MAX_FRAMES_IN_FLIGHT = 2;
    const static int textureCount{ 2 };
    static const int mariosToPaint = 1;

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
    VkDescriptorSetLayout descriptorSetLayoutSphere;
    VkDescriptorSetLayout descriptorSetLayoutNormal;
    VkPipelineLayout pipelineLayout;
    VkPipelineLayout pipelineLayoutNormal;
    VkPipeline graphicsPipelineID;
    VkPipeline graphicsPipelineIDNormal;
    std::array<VkDescriptorPool, textureCount> descriptorPoolArray;
    VkDescriptorPool descriptorPoolSphere;
    VkDescriptorPool descriptorPoolGeom;
    std::array<std::vector<VkDescriptorSet>, textureCount> descriptorSetsArray;
    std::vector<VkDescriptorSet> descriptorSetsSphere;
    std::vector<VkDescriptorSet> descriptorSetsNormal;

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

    ModelMatrixPushConst modelMatrixPushConst[mariosToPaint];
    ModelMatrixPushConst spherePushConst;



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
    void createDescriptorSetLayoutGeom(VkDescriptorSetLayout& descriptorSetLayout);
    void createGraphicsPipeline(const char* vFilename, const char* fFilename, VkPipeline& pipeline, VkPipelineLayout& pipelineLayout);
    void createGraphicsPipelineGeom(const char* vFilename, const char* gFilename, const char* fFilename, VkPipeline& pipeline, VkPipelineLayout& pipelineLayout, VkDescriptorSetLayout& descriptorSetLayout);
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
    void createDescriptorPoolGeom(VkDescriptorPool& descriptorPool);
    void createDescriptorSets(VkDescriptorSetLayout& descriptorSetLayout, std::vector<VkDescriptorSet>& descriptorSets, VkDescriptorPool descriptorPool, VkImageView& textureImageView, VkSampler& textureSample);
    void createDescriptorSetsGeom(VkDescriptorSetLayout& descriptorSetLayout, std::vector<VkDescriptorSet>& descriptorSets, VkDescriptorPool descriptorPool);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void createCommandBuffers();

    void recordCommandBuffer();
    void createSyncObjects();
    void cleanup();
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

    std::array<VkImage, textureCount>textureImageArray;
    VkImage textureImageSphere;
    VkDeviceMemory textureImageMemory;
    std::array<VkImageView,textureCount> textureImageViewArray;
    VkImageView textureImageViewSphere;
    std::array<VkSampler, textureCount> textureSamplerArray;
    VkSampler textureSamplerSphere;


    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    VkShaderModule createShaderModule(const std::vector<char>& code);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    static std::vector<char> readFile(const std::string& filename);


};
#endif 

