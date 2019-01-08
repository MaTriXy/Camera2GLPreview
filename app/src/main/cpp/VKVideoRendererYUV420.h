#ifndef _VK_VIDEO_RENDERER_YUV_H_
#define _VK_VIDEO_RENDERER_YUV_H_

#include "VideoRenderer.h"
#include "vulkan_wrapper.h"

class VKVideoRendererYUV420 : public VideoRenderer
{
public:
    VKVideoRendererYUV420();
    virtual ~VKVideoRendererYUV420();

    virtual void init(ANativeWindow* window, size_t width, size_t height) override;
    virtual void render() override;
    virtual void updateFrame(const video_frame& frame) override;
    virtual void draw(uint8_t *buffer, size_t length, size_t width, size_t height, int rotation) override;
    virtual void applyFilter(int filter) override;
    virtual int getMaxFilter() override;
    virtual bool createTextures() override;
    virtual bool updateTextures() override;
    virtual void deleteTextures() override;
    virtual int createProgram(const char *pVertexSource, const char *pFragmentSource) override;

private:
    enum TextureType { tTexY, tTexU, tTexV };

    struct Vertex {
        float pos[3];
        float uv[2];
    };

    struct UniformBufferObject {
        float projection[16];
        float rotation[16];
        float scale[16];
    };

    UniformBufferObject m_ubo;

    struct VulkanTexture {
        VkSampler sampler;
        VkImage image;
        VkImageLayout imageLayout;
        VkDeviceMemory mem;
        VkImageView view;
        size_t width;
        size_t height;
        void* mapped;
    };

    struct VulkanDeviceInfo {
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkPhysicalDeviceMemoryProperties memoryProperties;
        VkDevice device;
        uint32_t queueFamilyIndex;

        VkSurfaceKHR surface;
        VkQueue queue;

        bool initialized;
    };
    VulkanDeviceInfo m_deviceInfo;

    struct VulkanSwapchainInfo {
        VkSwapchainKHR swapchain;
        uint32_t swapchainLength;

        VkExtent2D displaySize;
        VkFormat displayFormat;

        // array of frame buffers and views
        std::unique_ptr<VkFramebuffer[]> framebuffers;
        std::unique_ptr<VkImage[]> displayImages;
        std::unique_ptr<VkImageView[]> displayViews;
    };
    VulkanSwapchainInfo m_swapchainInfo;

    struct VulkanRenderInfo {
        VkRenderPass renderPass;
        VkCommandPool cmdPool;
        std::unique_ptr<VkCommandBuffer[]> cmdBuffer;
        uint32_t cmdBufferLen;
        VkSemaphore semaphore;
        VkFence fence;
    };
    VulkanRenderInfo m_render;

    struct VulkanGfxPipelineInfo {
        VkDescriptorSetLayout descLayout;
        VkDescriptorPool descPool;
        VkDescriptorSet descSet;
        VkPipelineLayout layout;
        VkPipelineCache cache;
        VkPipeline pipeline;
    };
    VulkanGfxPipelineInfo m_gfxPipeline;

    struct VulkanBufferInfo {
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;

        VkBuffer uboBuffer;
        VkDeviceMemory uboBufferMemory;
    };
    VulkanBufferInfo m_buffers;

    static const uint32_t kTextureCount = 3;
    const VkFormat kTextureFormat = VK_FORMAT_R8_UNORM;
    const TextureType texType[kTextureCount];
    struct VulkanTexture textures[kTextureCount];

    uint8_t *m_pBuffer;
    uint32_t m_indexCount;
    size_t m_length;

    void createDevice(ANativeWindow* platformWindow, VkApplicationInfo* appInfo);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                      VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    VkResult createDescriptorSet();
    VkResult createGraphicsPipeline(const char *pVertexSource, const char *pFragmentSource);
    void createFrameBuffers(VkRenderPass& renderPass, VkImageView depthView = VK_NULL_HANDLE);
    void createRenderPass();
    void createSwapChain();
    void createUniformBuffers();
    void createVertexBuffer();
    void createIndexBuffer();
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void updateUniformBuffers();
    bool mapMemoryTypeToIndex(uint32_t typeBits, VkFlags requirements_mask, uint32_t* typeIndex);
    void deleteSwapChain();
    void deleteGraphicsPipeline();
    void deleteBuffers();

    bool isInitialized();

    VkResult allocateMemoryTypeFromProperties(uint32_t typeBits, VkFlags requirements_mask,
                                              uint32_t* typeIndex);

    size_t getBufferOffset(struct VulkanTexture* texture, TextureType type, size_t width, size_t height);

    void setImageLayout(VkCommandBuffer cmdBuffer,
                        VkImage image,
                        VkImageLayout oldImageLayout,
                        VkImageLayout newImageLayout,
                        VkPipelineStageFlags srcStages,
                        VkPipelineStageFlags destStages);

    VkResult loadTexture(uint8_t *buffer, TextureType type, size_t width, size_t height,
                         struct VulkanTexture* texture, VkImageUsageFlags usage, VkFlags required_props);

};

#endif //_VK_VIDEO_RENDERER_YUV_H_
