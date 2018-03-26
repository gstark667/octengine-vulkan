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

void model_update(model_t *model)
{

}

void model_render(model_t *model, VkCommandBuffer commandBuffer)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    kBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &model->vertexBuffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, &model->indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
}
