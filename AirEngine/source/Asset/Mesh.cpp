#include "Asset/Mesh.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Command/Semaphore.h"
#include <Utils/Log.h>
#include "Utils/OrientedBoundingBox.h"
#include <rttr/registration>

RTTR_REGISTRATION
{
    rttr::registration::class_<AirEngine::Asset::Mesh>("AirEngine::Asset::Mesh");
}

void AirEngine::Asset::Mesh::OnLoad(Core::Graphic::Command::CommandBuffer* transferCommandBuffer)
{
    std::vector<glm::vec3> vertexPositions = std::vector<glm::vec3>();

    Assimp::Importer importer;
    importer.SetPropertyFloat("PP_GSN_MAX_SMOOTHING_ANGLE", 0);
    const aiScene* scene = importer.ReadFile(Path(), aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    Utils::Log::Exception(importer.GetErrorString(), !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode);

    // process ASSIMP's root node recursively
    aiMesh* mesh = scene->mMeshes[scene->mRootNode->mMeshes[0]];
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        VertexData vertexData;
        glm::vec3 vector;
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertexData.position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertexData.normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertexData.texCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertexData.tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertexData.bitangent = vector;
        }
        else
        {
            Utils::Log::Exception("Mesh do not contains uv.");
        }

        _vertices.push_back(vertexData);
        vertexPositions.emplace_back(vertexData.position);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            _indices.push_back(face.mIndices[j]);
    }
    importer.FreeScene();


    VkDeviceSize vertexBufferSize = sizeof(VertexData) * _vertices.size();
    VkDeviceSize indexBufferSize = sizeof(uint32_t) * _indices.size();

    Core::Graphic::Instance::Buffer stageVertexBuffer = Core::Graphic::Instance::Buffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stageVertexBuffer.WriteData(_vertices.data(), vertexBufferSize);
    _vertexBuffer = new Core::Graphic::Instance::Buffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    Core::Graphic::Instance::Buffer stageIndexBuffer = Core::Graphic::Instance::Buffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stageIndexBuffer.WriteData(_indices.data(), indexBufferSize);
    _indexBuffer = new Core::Graphic::Instance::Buffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


    transferCommandBuffer->Reset();
    transferCommandBuffer->BeginRecord(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    transferCommandBuffer->CopyBuffer(&stageVertexBuffer, _vertexBuffer);
    transferCommandBuffer->CopyBuffer(&stageIndexBuffer, _indexBuffer);
    transferCommandBuffer->EndRecord();
    transferCommandBuffer->Submit();

    _orientedBoundingBox = new Utils::OrientedBoundingBox(vertexPositions);

    transferCommandBuffer->WaitForFinish();
    transferCommandBuffer->Reset();

}

AirEngine::Asset::Mesh::Mesh()
	: AssetBase(true)
    , _orientedBoundingBox(nullptr)
    , _vertexBuffer(nullptr)
    , _indexBuffer(nullptr)
    , _vertices()
    , _indices()
{
}

AirEngine::Asset::Mesh::Mesh(Core::Graphic::Command::CommandBuffer* transferCommandBuffer, std::vector<VertexData> vertices, std::vector<uint32_t> indices)
    : AssetBase(false)
    , _orientedBoundingBox(nullptr)
    , _vertexBuffer(nullptr)
    , _indexBuffer(nullptr)
    , _vertices(vertices)
    , _indices(indices)
{
    VkDeviceSize vertexBufferSize = sizeof(VertexData) * _vertices.size();
    VkDeviceSize indexBufferSize = sizeof(uint32_t) * _indices.size();

    Core::Graphic::Instance::Buffer stageVertexBuffer = Core::Graphic::Instance::Buffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stageVertexBuffer.WriteData(_vertices.data(), vertexBufferSize);
    _vertexBuffer = new Core::Graphic::Instance::Buffer(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    Core::Graphic::Instance::Buffer stageIndexBuffer = Core::Graphic::Instance::Buffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stageIndexBuffer.WriteData(_indices.data(), indexBufferSize);
    _indexBuffer = new Core::Graphic::Instance::Buffer(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


    transferCommandBuffer->Reset();
    transferCommandBuffer->BeginRecord(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    transferCommandBuffer->CopyBuffer(&stageVertexBuffer, _vertexBuffer);
    transferCommandBuffer->CopyBuffer(&stageIndexBuffer, _indexBuffer);
    transferCommandBuffer->EndRecord();
    transferCommandBuffer->Submit({}, {}, {});

    std::vector<glm::vec3> vertexPositions = std::vector<glm::vec3>(vertices.size());
    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        vertexPositions[i] = vertices[i].position;
    }

    _orientedBoundingBox = new Utils::OrientedBoundingBox(vertexPositions);

    transferCommandBuffer->WaitForFinish();
    transferCommandBuffer->Reset();

}

AirEngine::Asset::Mesh::~Mesh()
{
    delete _orientedBoundingBox;
    delete _vertexBuffer;
    delete _indexBuffer;
}

AirEngine::Core::Graphic::Instance::Buffer& AirEngine::Asset::Mesh::VertexBuffer()
{
    return *_vertexBuffer;
}

AirEngine::Core::Graphic::Instance::Buffer& AirEngine::Asset::Mesh::IndexBuffer()
{
    return *_indexBuffer;
}

std::vector<AirEngine::Asset::VertexData>& AirEngine::Asset::Mesh::Vertices()
{
    return _vertices;
}

std::vector<uint32_t>& AirEngine::Asset::Mesh::Indices()
{
    return _indices;
}

AirEngine::Utils::OrientedBoundingBox& AirEngine::Asset::Mesh::OrientedBoundingBox()
{
    return *_orientedBoundingBox;
}
