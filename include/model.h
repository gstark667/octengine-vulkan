struct model_t
{
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    VkBuffer uniformBuffer;
    VkDeviceMemory uniformBufferMemory;
};

void model_update(model_t *model);
void model_render(model_t *model, VkCommandBuffer commandBuffer);
