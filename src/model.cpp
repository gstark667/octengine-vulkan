#include <vulkan/vulkan.h>

#include "model.h"
#include "util.h"

#include <iostream>


VkVertexInputBindingDescription vertex_get_binding_description()
{
    VkVertexInputBindingDescription bindingDescription;
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(vertex_t);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

VkVertexInputBindingDescription instance_get_binding_description()
{
    VkVertexInputBindingDescription bindingDescription;
    bindingDescription.binding = 1;
    bindingDescription.stride = sizeof(model_instance_t);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 9> vertex_get_attribute_descriptions()
{
    std::array<VkVertexInputAttributeDescription, 9> attributeDescriptions;
    // vertex data
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(vertex_t, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(vertex_t, normal);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(vertex_t, texCoord);

    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(vertex_t, weights);

    attributeDescriptions[4].binding = 0;
    attributeDescriptions[4].location = 4;
    attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SINT;
    attributeDescriptions[4].offset = offsetof(vertex_t, bones);

    // instance data
    attributeDescriptions[5].binding = 1;
    attributeDescriptions[5].location = 5;
    attributeDescriptions[5].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[5].offset = offsetof(model_instance_t, pos);

    attributeDescriptions[6].binding = 1;
    attributeDescriptions[6].location = 6;
    attributeDescriptions[6].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[6].offset = offsetof(model_instance_t, rot);

    attributeDescriptions[7].binding = 1;
    attributeDescriptions[7].location = 7;
    attributeDescriptions[7].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[7].offset = offsetof(model_instance_t, scale);

    attributeDescriptions[8].binding = 1;
    attributeDescriptions[8].location = 8;
    attributeDescriptions[8].format = VK_FORMAT_R32G32B32_SINT;
    attributeDescriptions[8].offset = offsetof(model_instance_t, textureIdx);

    return attributeDescriptions;
}

aiMatrix4x4 interpolate_scale(bone_t *bone, float time)
{
    aiVector3D scale;
    if (bone->scaleKeyframes.size() == 1)
    {
        scale = bone->scaleKeyframes[0].vec;
    }
    else
    {
        scale_keyframe_t *start(NULL), *end(NULL);
        for (size_t i = 0; i < bone->scaleKeyframes.size(); ++i)
        {
            if (bone->scaleKeyframes[i].time < time)
            {
                start = &bone->scaleKeyframes[i];
            }
            else if (bone->scaleKeyframes[i].time > time)
            {
                end = &bone->scaleKeyframes[i];
                break;
            }
        }

        if (start && !end)
        {
            scale = start->vec;
        }
        else if (end && !start)
        {
            scale = end->vec;
        }
        else
        {
            float delta = (time - (float)start->time) / ((float)end->time - (float)start->time);
            scale = (start->vec + delta * (end->vec - start->vec));
        }
    }
    aiMatrix4x4 mat;
    aiMatrix4x4::Scaling(scale, mat);
    return mat;
}

aiMatrix4x4 interpolate_rotation(bone_t *bone, float time)
{
    aiQuaternion rotation;
    if (bone->rotationKeyframes.size() == 1)
    {
        rotation = bone->rotationKeyframes[0].quat;
    }
    else
    {
        rotation_keyframe_t *start(NULL), *end(NULL);
        for (size_t i = 0; i < bone->rotationKeyframes.size(); ++i)
        {
            if (bone->rotationKeyframes[i].time < time)
            {
                start = &bone->rotationKeyframes[i];
            }
            else if (bone->rotationKeyframes[i].time > time)
            {
                end = &bone->rotationKeyframes[i];
                break;
            }
        }

        if (start && !end)
        {
            rotation = start->quat;
        }
        else if (end && !start)
        {
            rotation = end->quat;
        }
        else
        {
            float delta = (time - (float)start->time) / ((float)end->time - (float)start->time);
            aiQuaternion::Interpolate(rotation, start->quat, end->quat, delta);
            rotation.Normalize();
        }
    }
    aiMatrix4x4 mat(rotation.GetMatrix());
    return mat;
}

aiMatrix4x4 interpolate_position(bone_t *bone, float time)
{
    aiVector3D position;
    if (bone->positionKeyframes.size() == 1)
    {
        position = bone->positionKeyframes[0].vec;
    }
    else
    {
        position_keyframe_t *start(NULL), *end(NULL);
        for (size_t i = 0; i < bone->positionKeyframes.size(); ++i)
        {
            if (bone->positionKeyframes[i].time < time)
            {
                start = &bone->positionKeyframes[i];
            }
            else if (bone->positionKeyframes[i].time > time)
            {
                end = &bone->positionKeyframes[i];
                break;
            }
        }

        if (start && !end)
        {
            position = start->vec;
        }
        else if (end && !start)
        {
            position = end->vec;
        }
        else
        {
            float delta = (time - (float)start->time) / ((float)end->time - (float)start->time);
            position = (start->vec + delta * (end->vec - start->vec));
        }
    }
    aiMatrix4x4 mat;
    aiMatrix4x4::Translation(position, mat);
    return mat;
}

bone_t *model_load_node(model_t *model, aiNode *node)
{
    std::cout << node->mName.data << std::endl;
    bone_t *bone = NULL;
    for (size_t b = 0; b < model->bones.size(); ++b)
    {
        if (model->bones[b].name == node->mName.data)
        {
            bone = &model->bones[b];
        }
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        bone_t *child = model_load_node(model, node->mChildren[i]);
        if (child && bone)
            bone->children.push_back(child);
    }
    return bone;
}

void model_load(model_t *model, std::string path)
{
    path = "models/" + path;
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType
    );

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        for (unsigned int vert = 0; vert < scene->mMeshes[i]->mNumVertices; ++vert)
        {
            aiVector3D vertex = scene->mMeshes[i]->mVertices[vert];
            aiVector3D normals = scene->mMeshes[i]->mNormals[vert];
            aiVector3D **uv = scene->mMeshes[i]->mTextureCoords;
            vertex_t newVertex;
            newVertex.pos = {vertex[0], vertex[1], vertex[2]};
            newVertex.normal = {normals[0], normals[1], normals[2]};
            if (uv[0])
                newVertex.texCoord = {uv[0][vert][0], uv[0][vert][1]};
            else
                newVertex.texCoord = {0, 0};
            newVertex.weights = {0.0f, 0.0f, 0.0f, 0.0f};
            newVertex.bones = {-1, -1, -1, -1};
            model->vertices.push_back(newVertex);
        }

        for (unsigned int face = 0; face < scene->mMeshes[i]->mNumFaces; ++face)
        {
            for (unsigned int j = 0; j < 3; ++j)
            {
                model->indices.push_back(scene->mMeshes[i]->mFaces[face].mIndices[j]);
            }
        }

        for (unsigned int bone = 0; bone < scene->mMeshes[i]->mNumBones; ++bone)
        {
            aiBone *b = scene->mMeshes[i]->mBones[bone];
            std::cout << b->mName.data << std::endl;
            bone_t newBone;
            newBone.offset = b->mOffsetMatrix;
            newBone.pos = model->bones.size();
            newBone.name = b->mName.data;
            model->bones.push_back(newBone);

            for (unsigned int weight = 0; weight < scene->mMeshes[i]->mBones[bone]->mNumWeights; ++weight)
            {
                aiVertexWeight vertexWeight = scene->mMeshes[i]->mBones[bone]->mWeights[weight];
                model->vertices.at(vertexWeight.mVertexId).weights[model->vertices.at(vertexWeight.mVertexId).boneCount] = vertexWeight.mWeight;
                model->vertices.at(vertexWeight.mVertexId).bones[model->vertices.at(vertexWeight.mVertexId).boneCount] = bone;
                ++model->vertices.at(vertexWeight.mVertexId).boneCount;
            }
        }
    }

    for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
    {
        for (unsigned int chan = 0; chan < scene->mAnimations[i]->mNumChannels; ++chan)
        {
            aiNodeAnim *channel = scene->mAnimations[i]->mChannels[chan];
            bone_t *bone = NULL;
            for (size_t b = 0; b < model->bones.size(); ++b)
            {
                if (model->bones[b].name == channel->mNodeName.data)
                {
                    bone = &model->bones[b];
                    break;
                }
            }
            if (!bone)
                continue;
            std::cout << "found bone" << std::endl;
            for (unsigned int j = 0; j < channel->mNumScalingKeys; ++j)
            {
                bone->scaleKeyframes.push_back({channel->mScalingKeys[j].mValue, (float)channel->mScalingKeys[j].mTime});
            }
            for (unsigned int j = 0; j < channel->mNumRotationKeys; ++j)
            {
                bone->rotationKeyframes.push_back({channel->mRotationKeys[j].mValue, (float)channel->mRotationKeys[j].mTime});
            }
            for (unsigned int j = 0; j < channel->mNumPositionKeys; ++j)
            {
                bone->positionKeyframes.push_back({channel->mPositionKeys[j].mValue, (float)channel->mPositionKeys[j].mTime});
            }
        }
    }

    model->globalInverseTransform = scene->mRootNode->mTransformation;
    model->globalInverseTransform.Inverse();

    model_load_node(model, scene->mRootNode);
}

void model_create_vertex_buffer(model_t *model, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
{
    VkDeviceSize bufferSize = (sizeof(model->vertices[0]) * model->vertices.size());
    buffer_create(&model->vertexBuffer, device, physicalDevice, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize);
    buffer_copy(&model->vertexBuffer, device, physicalDevice, commandPool, graphicsQueue, model->vertices.data(), bufferSize);
}

void model_create_index_buffer(model_t *model, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue) {
    VkDeviceSize bufferSize = (sizeof(model->indices[0]) * model->indices.size());
    buffer_create(&model->indexBuffer, device, physicalDevice, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize);
    buffer_copy(&model->indexBuffer, device, physicalDevice, commandPool, graphicsQueue, model->indices.data(), bufferSize);
}

void model_create_instance_buffer(model_t *model, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
{
    VkDeviceSize bufferSize = model->instances.size();
    buffer_create(&model->instanceBuffer, device, physicalDevice, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize);
    buffer_create(&model->instanceStagingBuffer, device, physicalDevice, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferSize);

    if (bufferSize > 0)
    {
        VkCommandBuffer commandBuffer = begin_single_time_commands(device, commandPool);
        buffer_stage(&model->instanceBuffer, &model->instanceStagingBuffer, device, commandBuffer, model->instances.data(), bufferSize);
        buffer_inline_copy(&model->instanceBuffer, &model->instanceStagingBuffer, commandBuffer);
        end_single_time_commands(device, commandPool, graphicsQueue, commandBuffer);
    }
}

void model_copy_instances(model_t *model, std::vector<gameobject_t*>instances)
{
    while (instances.size() > model->instances.size())
        model->instances.push_back({});
    while (instances.size() < model->instances.size())
        model->instances.pop_back();

    for (size_t i = 0; i < instances.size(); ++i)
    {
        model->instances[i].pos = instances[i]->globalPos;
        model->instances[i].rot = instances[i]->globalRot;
        model->instances[i].scale = instances[i]->scale;
        model->instances[i].textureIdx = instances[i]->textureIdx;
    }
}

void model_copy_instance_buffer(model_t *model, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
{
    VkDeviceSize bufferSize = (sizeof(model_instance_t) * model->instances.size());
    VkCommandBuffer commandBuffer = begin_single_time_commands(device, commandPool);
    buffer_stage(&model->instanceBuffer, &model->instanceStagingBuffer, device, commandBuffer, model->instances.data(), bufferSize);
    buffer_inline_copy(&model->instanceBuffer, &model->instanceStagingBuffer, commandBuffer);
    end_single_time_commands(device, commandPool, graphicsQueue, commandBuffer);
}

void model_create_buffers(model_t *model, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue)
{
    std::cout << "bones: " << model->bones.size() << std::endl;
    model_create_vertex_buffer(model, device, physicalDevice, commandPool, graphicsQueue);
    model_create_index_buffer(model, device, physicalDevice, commandPool, graphicsQueue);
    model_create_instance_buffer(model, device, physicalDevice, commandPool, graphicsQueue);
}

void model_update_bone(model_t *model, bone_t *bone, float time, aiMatrix4x4 parentMatrix)
{
    if (bone == NULL)
        return;
    bone->matrix = parentMatrix
        * interpolate_position(bone, time)
        * interpolate_rotation(bone, time)
        * interpolate_scale(bone, time);
    for (size_t i = 0; i < bone->children.size(); ++i)
        model_update_bone(model, bone->children[i], time, bone->matrix);
    bone->matrix = model->globalInverseTransform * bone->matrix * bone->offset;
}

void model_update(model_t *model, float delta, bone_ubo_t *boneUBO)
{
    if (!boneUBO)
        return;
    model->time += delta;
    // TODO allign this with actual animation length
    while (model->time > 2.5f)
        model->time -= 2.5f;
    // TODO make this pick the actual root bone (instead of assuming 0)
    if (model->bones.size() > 0)
    {
        model_update_bone(model, &model->bones[0], model->time, aiMatrix4x4());
        size_t idx = 0;
        for (auto it = model->bones.begin(); it != model->bones.end(); ++it)
        {
            for (short y = 0; y < 4; ++y)
            {
                for (short x = 0; x < 4; ++x)
                {
                    boneUBO->bones[idx][y][x] = it->matrix[x][y];
                }
            }
            ++idx;
        }
    }
}

void model_render(model_t *model, VkCommandBuffer commandBuffer, pipeline_t *pipeline, descriptor_set_t *descriptorSet)
{
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->layout, 0, 1, &descriptorSet->descriptorSet, 0, NULL);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);

    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &model->vertexBuffer.buffer, offsets);
    vkCmdBindVertexBuffers(commandBuffer, 1, 1, &model->instanceBuffer.buffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, model->indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(commandBuffer, model->indices.size(), model->instances.size(), 0, 0, 0);

    std::cout << model->vertices.size() << std::endl;
}

void model_cleanup(model_t *model, VkDevice device)
{
    buffer_destroy(&model->vertexBuffer, device);
    buffer_destroy(&model->indexBuffer, device);
    buffer_destroy(&model->instanceBuffer, device);
    buffer_destroy(&model->instanceStagingBuffer, device);
}

