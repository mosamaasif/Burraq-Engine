#include <BRQ.h>

#include "VKInitializers.h"

namespace BRQ { namespace VK {

    VkInstance CreateInstance(const InstanceCreateInfo& info) {

        VkInstance instance = VK_NULL_HANDLE;

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = info.ApplicationName.data();
        appInfo.pEngineName = info.EngineName.data();
        appInfo.applicationVersion = info.ApplicationVersion;
        appInfo.engineVersion = info.EngineVersion;
        appInfo.apiVersion = info.ApiVersion;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        std::vector<const char*> layers = info.EnabledLayerNames;
        std::vector<const char*> extensions = info.EnabledExtensionNames;

#ifdef BRQ_DEBUG
        VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};

        debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugInfo.pfnUserCallback = VKDEBUG::debugCallback;

        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugInfo;

        layers.push_back("VK_LAYER_KHRONOS_validation");
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

        createInfo.ppEnabledLayerNames = layers.data();
        createInfo.enabledLayerCount = (U32)layers.size();
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledExtensionCount = (U32)extensions.size();

        VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance));

#ifdef BRQ_DEBUG
        PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        BRQ_CORE_ASSERT(func);
        VK_CHECK(func(instance, &debugInfo, nullptr, &VKDEBUG::s_DebugMessenger));
#endif

        return instance;
    }

    void DestroyInstance(VkInstance& instance) {

#ifdef BRQ_DEBUG
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        BRQ_CORE_ASSERT(func);
        func(instance, VKDEBUG::s_DebugMessenger, nullptr);
#endif
        if (instance) {

            vkDestroyInstance(instance, nullptr);
            instance = VK_NULL_HANDLE;
        }
    }

    VkSurfaceKHR CreateSurface(const VkInstance& instance, const SurfaceCreateInfo& info) {

        VkSurfaceKHR surface = VK_NULL_HANDLE;

        VkWin32SurfaceCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = info.WindowHandle;
        createInfo.hinstance = info.InstanceHandle;

        VK_CHECK(vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface));

        return surface;
    }

    VkFormat FindSupportedFormat(const VkPhysicalDevice& physicalDevice, const std::vector<VkFormat>& candidates, const VkImageTiling& tiling, const VkFormatFeatureFlags& features) {

        for (VkFormat format : candidates) {

            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {

                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {

                return format;
            }
        }
      
        return VK_FORMAT_UNDEFINED;
    }

    VkFormat FindDepthImageFormat(const VkPhysicalDevice& physicalDevice) {

        std::vector<VkFormat> formats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
        VkFormat format = FindSupportedFormat(physicalDevice, formats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

        BRQ_CORE_ASSERT(format != VK_FORMAT_UNDEFINED);

        return format;
    }

    void DestroySurface(const VkInstance& instance, VkSurfaceKHR& surface) {

        if (surface) {

            vkDestroySurfaceKHR(instance, surface, nullptr);
            surface = VK_NULL_HANDLE;
        }
    }

    Image CreateImage(const ImageCreateInfo& info) {

        Image image = {};

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.flags = info.Flags;
        imageInfo.imageType = info.ImageType;
        imageInfo.format = info.Format;
        imageInfo.extent = info.Extent;
        imageInfo.mipLevels = info.MipLevels;
        imageInfo.arrayLayers = info.ArrayLayers;
        imageInfo.samples = info.Samples;
        imageInfo.tiling = info.Tiling;
        imageInfo.usage = info.Usage;
        imageInfo.sharingMode = info.SharingMode;
        imageInfo.queueFamilyIndexCount = (U32)info.QueueFamilyIndices.size();
        imageInfo.pQueueFamilyIndices = info.QueueFamilyIndices.data();
        imageInfo.initialLayout = info.InitialLayout;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = info.MemoryUsage;

        image.ImageAllocation = VulkanMemoryAllocator::GetInstance()->CreateImage(imageInfo, allocInfo);

        return image;
    }

    void DestroyImage(Image& image) {

        VulkanMemoryAllocator::GetInstance()->DestroyImage(image.ImageAllocation);
        image.ImageAllocation.Allocation = nullptr;
        image.ImageAllocation.Image = VK_NULL_HANDLE;
    }

    VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface) {

        VkSurfaceCapabilitiesKHR capabilities = {};
        VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities));

        return capabilities;
    }

    std::vector<VkSurfaceFormatKHR> GetSurfaceFormat(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface) {

        U32 formatCount = 0;

        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr));

        BRQ_CORE_ASSERT(formatCount);

        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, &formats[0]));

        return std::move(formats);
    }

    std::vector<VkPresentModeKHR> GetPresentMode(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface) {

        U32 presentCount = 0;

        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentCount, nullptr));

        BRQ_CORE_ASSERT(presentCount);

        std::vector<VkPresentModeKHR> presentModes(presentCount);
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentCount, &presentModes[0]));

        return std::move(presentModes);
    }

    U32 GetQueueFamilyIndex(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, QueueType type) {

        U32 queueCount = 0;
        U32 index = VK_QUEUE_FAMILY_IGNORED;

        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, nullptr);

        std::vector<VkQueueFamilyProperties> queues(queueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, &queues[0]);

        for (U32 i = 0; i < queueCount; i++) {

            if (type == QueueType::Graphics) {

                if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {

                    VkBool32 presentSupport = false;
                    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

                    if (presentSupport) {

                        index = i;
                        break;
                    }
                }
            }
            else if (type == QueueType::Compute) {

                if (queues[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {

                    index = i;
                    break;
                }
            }
            else if (type == QueueType::Tranfer) {

                if (queues[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {

                    index = i;
                    break;
                }
            }
        }
        return index;
    }

    VkPhysicalDeviceProperties GetPhysicalDeviceProperties(const VkPhysicalDevice& physicalDevice) {

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        return properties;
    }

    VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(const VkPhysicalDevice& physicalDevice) {

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physicalDevice, &features);

        return features;
    }

    VkPhysicalDevice SeletePhysicalDevice(const VkInstance& instance, const VkSurfaceKHR& surface) {

        U32 deviceCount = 0;

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr));
        BRQ_CORE_ASSERT(deviceCount);

        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);

        VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, &physicalDevices[0]));

        for (const auto& device : physicalDevices) {

            VkPhysicalDeviceProperties props = GetPhysicalDeviceProperties(device);

            VkPhysicalDeviceFeatures features = GetPhysicalDeviceFeatures(device);

            VkPhysicalDeviceMemoryProperties memProps;
            vkGetPhysicalDeviceMemoryProperties(device, &memProps);

            U32 graphicsAndPresentation = GetQueueFamilyIndex(device, surface, QueueType::Graphics);

            if (graphicsAndPresentation == VK_QUEUE_FAMILY_IGNORED)
                continue;

            if (props.apiVersion < VK_API_VERSION_1_2)
                continue;

            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {

                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice) {

            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(physicalDevice, &props);

            BRQ_CORE_TRACE("Renderer: {}", props.deviceName);
        }
        else {

            BRQ_CORE_FATAL("Can't Select Discrete GPU!");
            BRQ_CORE_ASSERT(false);
        }

        return physicalDevice;
    }


    VkDevice CreateDevice(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, const DeviceCreateInfo& info) {

        VkDevice device = VK_NULL_HANDLE;

        std::vector<U32> queueIndices;
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        for (U64 i = 0; i < info.QueueTypes.size(); i++) {

            const auto& type = info.QueueTypes[i].first;

            U32 index = GetQueueFamilyIndex(physicalDevice, surface, type);

            queueIndices.push_back(index);
        }

        U64 index = 0;

        for (U32 queueFamily : queueIndices) {

            VkDeviceQueueCreateInfo queueInfo = {};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = queueFamily;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &info.QueueTypes[index++].second;
            queueCreateInfos.push_back(queueInfo);
        }

        std::vector<const char*> layers = info.EnabledLayerNames;
        std::vector<const char*> extensions = info.EnabledExtensionNames;

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = (U32)queueCreateInfos.size();
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.pEnabledFeatures = &info.EnabledFeatures;

#ifdef BRQ_DEBUG
        layers.push_back("VK_LAYER_KHRONOS_validation");
#endif
        extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        deviceCreateInfo.ppEnabledLayerNames = layers.data();
        deviceCreateInfo.enabledLayerCount = (U32)layers.size();
        deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
        deviceCreateInfo.enabledExtensionCount = (U32)extensions.size();

        VK_CHECK(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));

        return device;
    }

    void DestroyDevice(VkDevice& device) {

        if (device) {

            vkDestroyDevice(device, nullptr);
            device = VK_NULL_HANDLE;
        }
    }

    VkSurfaceFormatKHR ChooseSwapchainFormat(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface) {

        VkSurfaceFormatKHR format = {};

        auto formats = VK::GetSurfaceFormat(physicalDevice, surface);

        BRQ_CORE_ASSERT(formats.size());

        if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {

            format = formats[0];
            format.format = VK_FORMAT_B8G8R8A8_UNORM;
        }
        else {

            format.format = VK_FORMAT_UNDEFINED;

            for (const auto& candidate : formats) {

                switch (candidate.format) {

                case VK_FORMAT_B8G8R8A8_UNORM:
                case VK_FORMAT_R8G8B8A8_UNORM:
                    format = candidate;
                    break;

                default:
                    break;
                }
            }

            if (format.format == VK_FORMAT_UNDEFINED) {

                for (const auto& candidate : formats) {

                    switch (candidate.format) {

                    case VK_FORMAT_B8G8R8A8_SRGB:
                    case VK_FORMAT_R8G8B8A8_SRGB:
                        format = candidate;
                        break;

                    default:
                        break;
                    }

                }

                if (format.format == VK_FORMAT_UNDEFINED) {

                    format = formats[0];
                }
            }
        }

        return format;
    }

    VkPresentModeKHR ChooseSwapchainPresentMode(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface) {

        auto modes = VK::GetPresentMode(physicalDevice, surface);

        for (const auto& mode : modes) {

            if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {

                return mode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D ChooseSwapchainExtent(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, const Window* window) {

        auto capabilities = VK::GetSurfaceCapabilities(physicalDevice, surface);

        if (capabilities.currentExtent.width != UINT32_MAX) {

            return capabilities.currentExtent;
        }
        else {

            U32 height = window->GetHeight();
            U32 width = window->GetWidth();

            VkExtent2D extent = { (U32)width, (U32)height };

            extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, extent.width));
            extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, extent.height));

            return extent;
        }
    }

    ImageView CreateImageView(const VkDevice& device, const ImageViewCreateInfo& info) {

        ImageView view = {};

        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.flags = info.Flags;
        createInfo.image = info.Image;
        createInfo.viewType = info.ViewType;
        createInfo.format = info.Format;
        createInfo.components = info.Components;
        createInfo.subresourceRange = info.SubresourceRange;
        
        VK_CHECK(vkCreateImageView(device, &createInfo, nullptr, &view.ImageView));

        return view;
    }

    void DestroyImageView(const VkDevice& device, ImageView& view) {

        if (view.ImageView) {

            vkDestroyImageView(device, view.ImageView, nullptr);
            view.ImageView = VK_NULL_HANDLE;
        }
    }

    SwapchainResult CreateSwapchain(const VkDevice& device, const SwapchainCreateInfo& info) {

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = info.Surface;
        createInfo.minImageCount = info.MinImageCount;
        createInfo.imageFormat = info.SurfaceFormat.format;
        createInfo.imageColorSpace = info.SurfaceFormat.colorSpace;
        createInfo.imageExtent = info.ImageExtent;
        createInfo.imageArrayLayers = info.ImageArrayLayers;
        createInfo.imageUsage = info.ImageUsage;
        createInfo.imageSharingMode = info.ImageSharingMode;
        createInfo.preTransform = info.PreTransform;
        createInfo.compositeAlpha = info.CompositeAlpha;
        createInfo.presentMode = info.PresentMode;
        createInfo.clipped = info.Clipped;
        createInfo.oldSwapchain = info.OldSwapchain;

        VkSwapchainKHR swapchain = VK_NULL_HANDLE;

        std::vector<VkImage> images;
        std::vector<Image> swapchainImages;
        std::vector<ImageView> swapchainImageViews;

        U32 imageCount = info.MinImageCount;

        VK_CHECK(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain));

        VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr));
        images.resize(imageCount);
        swapchainImages.resize(imageCount);
        swapchainImageViews.resize(imageCount);

        VK_CHECK(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data()));

        for (U64 i = 0; i < images.size(); i++) {

            swapchainImages[i].ImageAllocation.Image = images[i];

            ImageViewCreateInfo viewInfo = {};
            viewInfo.Image = images[i];
            viewInfo.ViewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.Format = info.SurfaceFormat.format;
            viewInfo.Components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.Components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.Components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.Components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.SubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.SubresourceRange.baseMipLevel = 0;
            viewInfo.SubresourceRange.levelCount = 1;
            viewInfo.SubresourceRange.baseArrayLayer = 0;
            viewInfo.SubresourceRange.layerCount = 1;

            swapchainImageViews[i] = VK::CreateImageView(device, viewInfo);
        };

        SwapchainResult result;
        result.Swapchain = swapchain;
        result.SwapchainImages = std::move(swapchainImages);
        result.SwapchainImageViews = std::move(swapchainImageViews);

        return result;
    }

    void DestroySwapchain(const VkDevice& device, VkSwapchainKHR& swapchain) {

        if (swapchain) {

            vkDestroySwapchainKHR(device, swapchain, nullptr);
            swapchain = VK_NULL_HANDLE;
        }
    }

    SwapchainResult UpdateSwapchain(const VkDevice& device, VkSwapchainKHR& old, const SwapchainCreateInfo& info) {

        SwapchainResult result = CreateSwapchain(device, info);

        DestroySwapchain(device, old);

        return result;
    }

    VkFramebuffer CreateFramebuffer(const VkDevice& device, const FramebufferCreateInfo& info) {

        VkFramebufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = info.RenderPass;
        createInfo.attachmentCount = (U32)info.Attachments.size();
        createInfo.pAttachments = info.Attachments.data();
        createInfo.width = info.Width;
        createInfo.height = info.Height;
        createInfo.layers = info.Layers;


        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        VK_CHECK(vkCreateFramebuffer(device, &createInfo, nullptr, &framebuffer));

        return framebuffer;
    }

    void DestroyFramebuffer(const VkDevice& device, VkFramebuffer& framebuffer) {

        if (framebuffer) {

            vkDestroyFramebuffer(device, framebuffer, nullptr);
            framebuffer = VK_NULL_HANDLE;
        }
    }

    VkResult Present(const PresentInfo& info) {

        VkPresentInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        createInfo.pWaitSemaphores = &info.WaitSemaphore;
        createInfo.waitSemaphoreCount = 1;
        createInfo.swapchainCount = 1;
        createInfo.pSwapchains = &info.Swapchain;
        createInfo.pImageIndices = &info.ImageIndex;

        return vkQueuePresentKHR(info.PresentQueue, &createInfo);
    }

    VkAttachmentDescription GetAttachmentDescription(const AttachmentDescription& description) {

        VkAttachmentDescription info = {};
        info.format = description.Format;
        info.flags = description.Flags;
        info.samples = description.Samples;
        info.loadOp = description.LoadOp;
        info.storeOp = description.StoreOp;
        info.stencilLoadOp = description.StencilLoadOp;
        info.stencilStoreOp = description.StencilStoreOp;
        info.initialLayout = description.InitialLayout;
        info.finalLayout = description.FinalLayout;

        return info;
    }

    VkAttachmentReference GetAttachmentReference(const AttachmentReference& reference) {

        VkAttachmentReference info = {};
        info.attachment = reference.Attachment;
        info.layout = reference.Layout;

        return info;
    }

    VkSubpassDescription GetSubpassDescription(const SubpassDescription& description) {

        VkSubpassDescription info = {};
        info.flags = description.Flags;
        info.pipelineBindPoint = description.PipelineBindPoint;
        info.inputAttachmentCount = (U32)description.InputAttachments.size();
        info.pInputAttachments = description.InputAttachments.data();
        info.colorAttachmentCount = (U32)description.ColorAttachments.size();
        info.pColorAttachments = description.ColorAttachments.data();
        info.pResolveAttachments = description.ResolveAttachments.data();
        info.pDepthStencilAttachment = &description.DepthStencilAttachment;
        info.preserveAttachmentCount = (U32)description.PreserveAttachments.size();
        info.pPreserveAttachments = description.PreserveAttachments.data();

        return info;
    }

    VkSubpassDependency GetSubpassDependency(const SubpassDependency& dependency) {

        VkSubpassDependency info = {};
        info.srcSubpass = dependency.SrcSubpass;
        info.dstSubpass = dependency.DstSubpass;
        info.srcStageMask = dependency.SrcStageMask;
        info.dstStageMask = dependency.DstStageMask;
        info.srcAccessMask = dependency.SrcAccessMask;
        info.dstAccessMask = dependency.DstAccessMask;
        info.dependencyFlags = dependency.DependencyFlags;

        return info;
    }

    VkRenderPass CreateRenderPass(const VkDevice& device, const RenderPassCreateInfo& info) {

        VkRenderPassCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount = (U32)info.Attachments.size();
        createInfo.pAttachments = info.Attachments.data();
        createInfo.subpassCount = (U32)info.Subpasses.size();
        createInfo.pSubpasses = info.Subpasses.data();
        createInfo.dependencyCount = (U32)info.Dependencies.size();
        createInfo.pDependencies = info.Dependencies.data();

        VkRenderPass renderPass = VK_NULL_HANDLE;

        VK_CHECK(vkCreateRenderPass(device, &createInfo, nullptr, &renderPass));

        return renderPass;
    }

    void DestroyRenderPass(const VkDevice& device, VkRenderPass& renderPass) {

        if (renderPass) {

            vkDestroyRenderPass(device, renderPass, nullptr);
            renderPass = VK_NULL_HANDLE;
        }
    }

    VkPipelineLayout CreatePipelineLayout(const VkDevice& device, const PipelineLayoutCreateInfo& info) {

        VkPipelineLayoutCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        createInfo.setLayoutCount = (U32)info.SetLayouts.size();
        createInfo.pSetLayouts = info.SetLayouts.data();
        createInfo.pushConstantRangeCount = (U32)info.PushConstantRanges.size();
        createInfo.pPushConstantRanges = info.PushConstantRanges.data();

        VkPipelineLayout layout = VK_NULL_HANDLE;

        VK_CHECK(vkCreatePipelineLayout(device, &createInfo, nullptr, &layout));

        return layout;
    }

    void DestroyPipelineLayout(const VkDevice& device, VkPipelineLayout& layout) {

        if (layout) {

            vkDestroyPipelineLayout(device, layout, nullptr);
            layout = VK_NULL_HANDLE;
        }
    }

    VkPipeline CreateGraphicsPipeline(const VkDevice& device, const GraphicsPipelineCreateInfo info) {

        VkGraphicsPipelineCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.flags = info.Flags;
        createInfo.stageCount = (U32)info.Stages.size();
        createInfo.pStages = info.Stages.data();
        createInfo.pVertexInputState = &info.VertexInputState;
        createInfo.pInputAssemblyState = &info.InputAssemblyState;
        createInfo.pTessellationState = &info.TessellationState;
        createInfo.pViewportState = &info.ViewportState;
        createInfo.pRasterizationState = &info.RasterizationState;
        createInfo.pMultisampleState = &info.MultisampleState;
        createInfo.pDepthStencilState = &info.DepthStencilState;
        createInfo.pColorBlendState = &info.ColorBlendState;
        createInfo.pDynamicState = &info.DynamicState;
        createInfo.layout = info.Layout;
        createInfo.renderPass = info.RenderPass;
        createInfo.subpass = info.Subpass;
        createInfo.basePipelineHandle = info.BasePipelineHandle;
        createInfo.basePipelineIndex = info.BasePipelineIndex;

        VkPipeline pipeline = VK_NULL_HANDLE;

        VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline));

        return pipeline;
    }

    std::vector<VkPipeline> CreateGraphicsPipelines(const VkDevice& device, const std::vector<GraphicsPipelineCreateInfo>& infos) {

        std::vector<VkPipeline> results(infos.size());
        std::vector<VkGraphicsPipelineCreateInfo> createInfos(infos.size());

        for (U64 i = 0; i < infos.size(); i++) {

            VkGraphicsPipelineCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            createInfo.flags = infos[i].Flags;
            createInfo.stageCount = (U32)infos[i].Stages.size();
            createInfo.pStages = infos[i].Stages.data();
            createInfo.pVertexInputState = &infos[i].VertexInputState;
            createInfo.pInputAssemblyState = &infos[i].InputAssemblyState;
            createInfo.pTessellationState = &infos[i].TessellationState;
            createInfo.pViewportState = &infos[i].ViewportState;
            createInfo.pRasterizationState = &infos[i].RasterizationState;
            createInfo.pMultisampleState = &infos[i].MultisampleState;
            createInfo.pDepthStencilState = &infos[i].DepthStencilState;
            createInfo.pColorBlendState = &infos[i].ColorBlendState;
            createInfo.pDynamicState = &infos[i].DynamicState;
            createInfo.layout = infos[i].Layout;
            createInfo.renderPass = infos[i].RenderPass;
            createInfo.subpass = infos[i].Subpass;
            createInfo.basePipelineHandle = infos[i].BasePipelineHandle;
            createInfo.basePipelineIndex = infos[i].BasePipelineIndex;

            createInfos[i] = createInfo;
        }

        // TODO: PipelineCache, its crucial for performance!

        VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, (U32)createInfos.size(), createInfos.data(), nullptr, results.data()));

        return std::move(results);
    }

    void DestroyGraphicsPipeline(const VkDevice& device, VkPipeline& pipeline) {

        if (pipeline) {

            vkDestroyPipeline(device, pipeline, nullptr);
            pipeline = VK_NULL_HANDLE;
        }
    }

    void DestroyGraphicsPipelines(const VkDevice& device, std::vector<VkPipeline>& pipelines) {

        for (auto& pipeline : pipelines) {

            DestroyGraphicsPipeline(device, pipeline);
        }

        pipelines.clear();
    }

    VkSemaphore CreateVKSemaphore(const VkDevice device, const SemaphoreCreateInfo& info) {

        VkSemaphoreCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        createInfo.flags = info.Flags;

        VkSemaphore semaphore = VK_NULL_HANDLE;

        VK_CHECK(vkCreateSemaphore(device, &createInfo, nullptr, &semaphore));

        return semaphore;
    }

    void DestroyVKSemaphore(const VkDevice device, VkSemaphore semaphore) {

        if (semaphore) {

            vkDestroySemaphore(device, semaphore, nullptr);
            semaphore = VK_NULL_HANDLE;
        }
    }

    VkFence CreateFence(const VkDevice& device, const FenceCreateInfo& info) {
    
        VkFenceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        createInfo.flags = info.Flags;

        VkFence fence = VK_NULL_HANDLE;
        VK_CHECK(vkCreateFence(device, &createInfo, nullptr, &fence));

        return fence;
    }

    void DestroyFence(const VkDevice& device, VkFence& fence) {
        
        if (fence) {

            vkDestroyFence(device, fence, nullptr);
            fence = VK_NULL_HANDLE;
        }
    }

    void WaitForFence(const VkDevice& device, const VkFence& fence) {

        VK_CHECK(vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX));
    }

    void ResetFence(const VkDevice& device, const VkFence& fence) {

        VK_CHECK(vkResetFences(device, 1, &fence));
    }

    void WaitForFences(const VkDevice& device, const std::vector<VkFence>& fence) {

        VK_CHECK(vkWaitForFences(device, (U32)fence.size(), fence.data(), VK_TRUE, UINT64_MAX));
    }

    void ResetFences(const VkDevice& device, const std::vector<VkFence>& fence) {

        VK_CHECK(vkResetFences(device, (U32)fence.size(), fence.data()));
    }

    void QueueSubmit(const QueueSubmitInfo& info) {

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = info.WaitSemaphoreCount;
        submitInfo.pWaitSemaphores = info.WaitSemaphores;
        submitInfo.pWaitDstStageMask = &info.WaitDstStageMask;
        submitInfo.commandBufferCount = info.CommandBufferCount;
        submitInfo.pCommandBuffers = info.CommandBuffers;
        submitInfo.signalSemaphoreCount = info.SignalSemaphoreCount;
        submitInfo.pSignalSemaphores = info.SignalSemaphores;

        VK_CHECK(vkQueueSubmit(info.Queue, 1, &submitInfo, info.CommandBufferExecutedFence));
    }

    void QueueWaitIdle(const VkQueue& queue) {

        VK_CHECK(vkQueueWaitIdle(queue));
    }

    VkCommandPool CreateCommandPool(const VkDevice& device, const CommandPoolCreateInfo& info) {

        VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.flags = info.Flags;
        createInfo.queueFamilyIndex = info.QueueFamilyIndex;

        VkCommandPool pool = VK_NULL_HANDLE;

        VK_CHECK(vkCreateCommandPool(device, &createInfo, nullptr, &pool));

        return pool;
    }

    void DestroyCommandPool(const VkDevice& device, VkCommandPool& pool) {
        
        if (pool) {

            vkDestroyCommandPool(device, pool, nullptr);
            pool = VK_NULL_HANDLE;
        }
    }

    void ResetCommandPool(const VkDevice& device, const VkCommandPool& pool, VkCommandPoolResetFlags resetFlags) {

        VK_CHECK(vkResetCommandPool(device, pool, resetFlags));
    }

    std::vector<VkCommandBuffer> AllocateCommandBuffers(const VkDevice& device, const CommandBufferAllocateInfo& info) {

        std::vector<VkCommandBuffer> result(info.CommandBufferCount);

        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = info.CommandPool;
        allocateInfo.level = info.Level;
        allocateInfo.commandBufferCount = info.CommandBufferCount;

        VK_CHECK(vkAllocateCommandBuffers(device, &allocateInfo, result.data()));

        return std::move(result);
    }

    void FreeCommandBuffer(const VkDevice& device, const VkCommandPool& pool, VkCommandBuffer& buffer) {

        vkFreeCommandBuffers(device, pool, 1, &buffer);
        buffer = VK_NULL_HANDLE;
    }

    void CommandBufferBegin(const CommandBufferBeginInfo& info) {

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = info.Flags;
        beginInfo.pInheritanceInfo = info.InheritanceInfo;

        VK_CHECK(vkBeginCommandBuffer(info.CommandBuffer, &beginInfo));
    }

    void CommandBufferEnd(const VkCommandBuffer& buffer) {

        VK_CHECK(vkEndCommandBuffer(buffer));
    }

    Buffer CreateBuffer(const BufferCreateInfo& info) {

        Buffer result = {};

        auto vma = VulkanMemoryAllocator::GetInstance();

        bool needStagingBuffer = info.MemoryUsage == VMA_MEMORY_USAGE_GPU_ONLY;

        VkBufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.flags = info.Flags;
        createInfo.size = info.Size;
        createInfo.usage = info.Usage;
        createInfo.sharingMode = info.SharingMode;
        createInfo.queueFamilyIndexCount = (U32)info.QueueFamilyIndices.size();
        createInfo.pQueueFamilyIndices = info.QueueFamilyIndices.data();

        VmaAllocationCreateInfo allocationInfo = {};
        allocationInfo.flags = info.MemoryFlags;
        allocationInfo.usage = info.MemoryUsage;

        if (needStagingBuffer) {

            createInfo.usage = info.Usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }

        result.BufferAllocation = vma->CreateBuffer(createInfo, allocationInfo);
        return result;
    }

    void DestoryBuffer(Buffer& buffer) {

        auto vma = VulkanMemoryAllocator::GetInstance();

        vma->DestroyBuffer(buffer.BufferAllocation);
    }

    void UploadBuffer(const VkDevice& device, const UploadBufferInfo& info) {

        bool needStagingBuffer = info.MemoryUsage == VMA_MEMORY_USAGE_GPU_ONLY;

        auto vma = VulkanMemoryAllocator::GetInstance();

        if (needStagingBuffer) {

            VkBufferCreateInfo stagingCreateInfo = {};
            stagingCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            stagingCreateInfo.size = info.Size;
            stagingCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            stagingCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            VmaAllocationCreateInfo stagingAllocationCreateInfo = {};
            stagingAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
            stagingAllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

            auto stagingBuffer = vma->CreateBuffer(stagingCreateInfo, stagingAllocationCreateInfo);
            auto stagingInfo = vma->GetAllocationInfo(stagingBuffer);

            memcpy_s(stagingInfo.pMappedData, stagingInfo.size, info.Data, info.Size);

            CommandBufferBeginInfo beginInfo = {};
            beginInfo.CommandBuffer = info.CommandBuffer;
            beginInfo.Flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            CommandBufferBegin(beginInfo);

            VkBufferCopy copyRegion = {};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = info.Size;
            vkCmdCopyBuffer(info.CommandBuffer, stagingBuffer.Buffer, info.DestinationBuffer->BufferAllocation.Buffer, 1, &copyRegion);

            CommandBufferEnd(info.CommandBuffer);

            VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &info.CommandBuffer;

            VK_CHECK(vkQueueSubmit(info.Queue, 1, &submitInfo, VK_NULL_HANDLE));

            if (info.WaitForUpload) {

                VK_CHECK(vkQueueWaitIdle(info.Queue));
            }

            vma->DestroyBuffer(stagingBuffer);
        }
        else {

            void* data = vma->MapMemory(info.DestinationBuffer->BufferAllocation);

            memcpy_s(data, info.Size, info.Data, info.Size);

            vma->UnMapMemory(info.DestinationBuffer->BufferAllocation);
        }
    }

    VkDescriptorSetLayout CreateDescriptorSetLayout(const VkDevice& device, const DescriptorSetLayoutCreateInfo& info) {

        VkDescriptorSetLayout layout = VK_NULL_HANDLE;

        VkDescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        createInfo.flags = info.Flags;
        createInfo.bindingCount = info.BindingCount;
        createInfo.pBindings = info.Bindings;

        VK_CHECK(vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &layout));

        return layout;
    }

    void DestoryDescriptorSetLayout(const VkDevice& device, VkDescriptorSetLayout& layout) {

        vkDestroyDescriptorSetLayout(device, layout, nullptr);
        layout = VK_NULL_HANDLE;
    }

    VkDescriptorPool CreateDescriptorPool(const VkDevice& device, const DescriptorPoolCreateInfo& info) {

        VkDescriptorPool pool = VK_NULL_HANDLE;

        VkDescriptorPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createInfo.flags = info.Flags;
        createInfo.maxSets = info.MaxSets;
        createInfo.poolSizeCount = info.PoolSizeCount;
        createInfo.pPoolSizes = info.PoolSizes;

        VK_CHECK(vkCreateDescriptorPool(device, &createInfo, nullptr, &pool));

        return pool;
    }

    void DestoryDescriptorPool(const VkDevice& device, VkDescriptorPool& pool) {

        vkDestroyDescriptorPool(device, pool, nullptr);
        pool = VK_NULL_HANDLE;
    }

    std::vector<VkDescriptorSet> AllocateDescriptorSets(const VkDevice& device, const DescriptorSetAllocateInfo& info) {

        VkDescriptorSetAllocateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        createInfo.descriptorPool = info.DescriptorPool;
        createInfo.descriptorSetCount = info.DescriptorSetCount;
        createInfo.pSetLayouts = info.SetLayouts;

        std::vector<VkDescriptorSet> sets(info.DescriptorSetCount);

        VK_CHECK(vkAllocateDescriptorSets(device, &createInfo, sets.data()));

        return std::move(sets);
    }

    void ImageLayoutTransition(const ImageLayoutTransitionInfo info) {

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = info.OldLayout;
        barrier.newLayout = info.NewLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = info.Image;
        barrier.subresourceRange = info.SubresourceRange;

        VkPipelineStageFlags sourceStage = {};
        VkPipelineStageFlags destinationStage = {};

        if (info.OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && info.NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {

            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (info.OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && info.NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {

            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else {

            BRQ_ASSERT("THIS DOESNT WORK");
        }

        vkCmdPipelineBarrier(
            info.CommandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
    }

    VkSampler CreateSampler(const VkDevice& device, const SamplerCreateInfo& info) {

        VkSampler sampler = VK_NULL_HANDLE;

        VkSamplerCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        createInfo.flags = info.Flags;
        createInfo.magFilter = info.MagFilter;
        createInfo.minFilter = info.MinFilter;
        createInfo.mipmapMode = info.MipmapMode;
        createInfo.addressModeU = info.AddressModeU;
        createInfo.addressModeV = info.AddressModeV;
        createInfo.addressModeW = info.AddressModeW;
        createInfo.mipLodBias = info.MipLodBias;
        createInfo.anisotropyEnable = info.AnisotropyEnable;
        createInfo.maxAnisotropy = info.MaxAnisotropy;
        createInfo.compareEnable = info.CompareEnable;
        createInfo.compareOp = info.CompareOp;
        createInfo.minLod = info.MinLod;
        createInfo.maxLod = info.MaxLod;
        createInfo.borderColor = info.BorderColor;
        createInfo.unnormalizedCoordinates = info.UnnormalizedCoordinates;

        VK_CHECK(vkCreateSampler(device, &createInfo, nullptr, &sampler));

        return sampler;
    }

    void DestroySampler(const VkDevice& device, VkSampler& sampler) {

        vkDestroySampler(device, sampler, nullptr);
        sampler = VK_NULL_HANDLE;
    }

} }