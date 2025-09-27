#include "queue.h"

#include "physicalDevice.h"
#include "device.h"

#include "logger.h"

uint32_t distinctQueueFamilyCount;
VkDeviceQueueCreateInfo *queueInfos;

Queue graphicsQueue;
Queue  computeQueue;
Queue transferQueue;

uint32_t queueCount;
Queue *queueReferences[] = {
    &graphicsQueue,
    & computeQueue,
    &transferQueue
};

void generateQueueDetails() {
    queueCount = sizeof(queueReferences) / sizeof(Queue *);
    debug("Queue count: %d",  queueCount);

    graphicsQueue.requiredFlags = VK_QUEUE_GRAPHICS_BIT;
     computeQueue.requiredFlags = VK_QUEUE_COMPUTE_BIT ;
    transferQueue.requiredFlags = VK_QUEUE_TRANSFER_BIT;

    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        uint32_t mostSuitedScore = 0;
        uint32_t mostSuitedIndex = UINT32_MAX;

        for(uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++) {
            VkQueueFlags supportedFlags = queueFamilyProperties[queueFamilyIndex].queueFlags;
            VkQueueFlags satisfiedFlags = supportedFlags &  queueReferences[queueIndex]->requiredFlags;
            VkQueueFlags redundantFlags = supportedFlags & ~queueReferences[queueIndex]->requiredFlags;

            uint32_t queueFamilyScore = stdc_count_zeros_ui(redundantFlags);

            if(queueReferences[queueIndex]->requiredFlags == satisfiedFlags && mostSuitedScore < queueFamilyScore) {
                mostSuitedScore = queueFamilyScore;
                mostSuitedIndex = queueFamilyIndex;
            }
        }

        queueReferences[queueIndex]->queueInfoIndex = 0;
        queueReferences[queueIndex]->queueIndex = 0;
        queueReferences[queueIndex]->queueFamilyIndex = mostSuitedIndex;

        assert(queueReferences[queueIndex]->queueFamilyIndex < queueFamilyCount);
    }

    distinctQueueFamilyCount = 1;

    for(uint32_t queueIndex = 1; queueIndex < queueCount; queueIndex++) {
        VkBool32 queueDistinct = VK_TRUE;

        for(int32_t comparisonIndex = (int32_t) queueIndex - 1; comparisonIndex >= 0; comparisonIndex--) {
            if(queueReferences[queueIndex]->queueFamilyIndex == queueReferences[comparisonIndex]->queueFamilyIndex) {
                queueReferences[queueIndex]->queueInfoIndex = queueReferences[comparisonIndex]->queueInfoIndex;
                queueReferences[queueIndex]->queueIndex = queueReferences[comparisonIndex]->queueIndex + 1;

                if(queueReferences[queueIndex]->queueIndex >= queueFamilyProperties[queueReferences[queueIndex]->queueFamilyIndex].queueCount) {
                    queueReferences[queueIndex]->queueIndex = queueFamilyProperties[queueReferences[queueIndex]->queueFamilyIndex].queueCount - 1;
                }

                queueDistinct = VK_FALSE;
                break;
            }
        }

        if(queueDistinct) {
            queueReferences[queueIndex]->queueInfoIndex = distinctQueueFamilyCount++;
        }
    }

    debug("Distinct queue family count: %d", distinctQueueFamilyCount);

    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        debug("Queue %d:", queueIndex);
        debug("\tQueue Family Index: %d", queueReferences[queueIndex]->queueFamilyIndex);
        debug("\tQueue Index:        %d", queueReferences[queueIndex]->queueIndex);
        debug("\tQueue Info Index:   %d", queueReferences[queueIndex]->queueInfoIndex);
    }
}

void getQueues() {
    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        Queue *queueReference = queueReferences[queueIndex];

        vkGetDeviceQueue(device, queueReference->queueFamilyIndex, queueReference->queueIndex, &queueReference->queue);
        debug("Device queue %d retrieved", queueIndex);

        VkCommandPoolCreateInfo commandPoolInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queueReference->queueFamilyIndex
        };

        vkCreateCommandPool(device, &commandPoolInfo, nullptr, &queueReference->commandPool);
        debug("\tCommand pool created");
    }
}

VkCommandBuffer allocateSingleCommandBuffer(Queue *queue) {
    VkCommandBufferAllocateInfo commandBufferInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = queue->commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &commandBufferInfo, &commandBuffer);
    return commandBuffer;
}

void beginSingleCommand(VkCommandBuffer *commandBuffer) {
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr
    };

    vkBeginCommandBuffer(*commandBuffer, &beginInfo);
}

VkCommandBuffer beginSingleTransferCommand() {
    VkCommandBuffer commandBuffer = allocateSingleCommandBuffer(&transferQueue);

    beginSingleCommand(&commandBuffer);

    return commandBuffer;
}

VkCommandBuffer beginSingleGraphicsCommand() {
    VkCommandBuffer commandBuffer = allocateSingleCommandBuffer(&graphicsQueue);

    beginSingleCommand(&commandBuffer);

    return commandBuffer;
}

void endSingleCommand(Queue *queue, VkCommandBuffer *commandBuffer) {
    vkEndCommandBuffer(*commandBuffer);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = nullptr,
        .commandBufferCount = 1,
        .pCommandBuffers = commandBuffer,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = nullptr
    };

    vkQueueSubmit(queue->queue, 1, &submitInfo, nullptr);
    vkQueueWaitIdle(queue->queue);
    vkFreeCommandBuffers(device, queue->commandPool, 1, commandBuffer);
}

void endSingleTransferCommand(VkCommandBuffer commandBuffer) {
    endSingleCommand(&transferQueue, &commandBuffer);
}

void endSingleGraphicsCommand(VkCommandBuffer commandBuffer) {
    endSingleCommand(&graphicsQueue, &commandBuffer);
}

void clearQueues() {
    for(uint32_t queueIndex = 0; queueIndex < queueCount; queueIndex++) {
        vkQueueWaitIdle(queueReferences[queueIndex]->queue);
        vkDestroyCommandPool(device, queueReferences[queueIndex]->commandPool, nullptr);
        debug("Command pool %d destroy", queueIndex);
    }
}
