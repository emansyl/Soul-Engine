#include "VulkanDevice.h"
#include "Parallelism/Fiber/Scheduler.h"

VulkanDevice::VulkanDevice(Scheduler& scheduler, int graphicsIndex, int presentIndex, vk::PhysicalDevice* physicalDevice, vk::Device device) :
	scheduler_(&scheduler),
	device_(device),
	physicalDevice_(physicalDevice)
{

	graphicsQueue_ = device.getQueue(graphicsIndex,0);
	presentQueue_ = device.getQueue(presentIndex,0);

	vk::PipelineCacheCreateInfo pipelineCreateInfo;
	//TODO: pipeline serialization n' such

	pipelineCache_ = device_.createPipelineCache(pipelineCreateInfo);

	vk::CommandPoolCreateInfo poolInfo;
	poolInfo.queueFamilyIndex = graphicsIndex;

	//TODO: move to 3 commandpools per thread as suggested by NVIDIA
	scheduler_->ForEachThread(FiberPriority::UX, [this, poolInfo]()
	{

		commandPool_ = device_.createCommandPool(poolInfo);

	});


}

void VulkanDevice::Terminate() {

	device_.destroyPipelineCache(pipelineCache_);

	scheduler_->ForEachThread(FiberPriority::UX, [this]()
	{

		device_.destroyCommandPool(commandPool_);

	});

	device_.destroy();

}

const vk::Device& VulkanDevice::GetLogicalDevice() const {
	return device_;
}

const vk::PipelineCache& VulkanDevice::GetPipelineCache() const {
	return pipelineCache_;
}

const vk::PhysicalDevice& VulkanDevice::GetPhysicalDevice() const {
	return *physicalDevice_;
}

const vk::CommandPool& VulkanDevice::GetCommandPool() const {
	return commandPool_;
}

const vk::Queue& VulkanDevice::GetGraphicsQueue() const {
	return graphicsQueue_;
}

const vk::Queue& VulkanDevice::GetPresentQueue() const {
	return presentQueue_;
}