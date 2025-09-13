#include "Model.h"
#include <iostream>
#include <GL/glew.h>

// این خط باید فقط در یک فایل cpp باشد تا پیاده‌سازی stb_image انجام شود
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Model::Model(std::string const &path)
{
    loadModel(path); // در سازنده، مدل را بارگذاری کن
}

void Model::Draw(Shader &shader)
{
    // هر مش را به ترتیب رسم کن
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

void Model::loadModel(std::string const &path)
{
    Assimp::Importer importer;
    // خواندن مدل از فایل
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    // بررسی خطا
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    // دایرکتوری فایل را ذخیره کن
    directory = path.substr(0, path.find_last_of('/'));
    
    // پردازش نودهای صحنه به صورت بازگشتی
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // پردازش تمام مش‌های موجود در نود فعلی
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // پردازش تمام فرزندان نود فعلی
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // پردازش رئوس مش
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // موقعیت
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // نرمال
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        } else {
            // اگر نرمال نداشت، نرمال پیش فرض صفر بده (که باعث مشکل در نورپردازی می‌شود)
            vertex.Normal = glm::vec3(0.0f);
        }
        // مختصات بافت
        if (mesh->mTextureCoords[0]) // آیا مش مختصات بافت دارد؟
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f); // مختصات بافت پیش‌فرض
        vertices.push_back(vertex);
    }

    // پردازش ایندکس‌ها
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // پردازش متریال (مواد)
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
    // ۱. Diffuse maps
    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // ۲. Specular maps
    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // ۳. Normal maps (اگر مدل داشته باشد)
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal"); // Assimp برای normal از HEIGHT استفاده می‌کند
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());


    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        // بررسی کنید آیا بافت از قبل بارگذاری شده است یا خیر (برای بهینه‌سازی)
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // بافت از قبل لود شده است، از لود مجدد صرف نظر کن
                break;
            }
        }
        if (!skip)
        {   // اگر بافت لود نشده بود، آن را لود کن
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // آن را به لیست بافت‌های لود شده اضافه کن
        }
    }
    return textures;
}

// ===================================================================================
// تابع کمکی برای بارگذاری عکس (بافت) از فایل
// این تابع خارج از کلاس Model قرار دارد تا بتواند توسط سایر بخش‌ها هم استفاده شود
// ===================================================================================
unsigned int TextureFromFile(const char *path, const std::string &directory)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename; // مسیر کامل فایل بافت

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true); // عکس را بچرخان
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << filename << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}