#include "Mesh.h"
#include <GL/glew.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh(); // پس از دریافت داده‌ها، OpenGL buffers را راه‌اندازی کن
}

void Mesh::setupMesh()
{
    // ساخت VAO, VBO, EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // ارسال داده‌های رئوس به GPU
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // ارسال داده‌های ایندکس به GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // تنظیم پوینترهای ورتکس اَتریبیوت (Vertex Attribute Pointers)
    // aPos (موقعیت)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // aNormal (نرمال)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // aTexCoords (مختصات بافت)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0); // از VAO جدا شو
}

void Mesh::Draw(Shader &shader)
{
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    // فعال کردن بافت‌ها قبل از رسم
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // فعال کردن Texture Unit مناسب
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);

        // ارسال یونیفرم به شیدر (مثلاً texture_diffuse1)
        shader.setInt((name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id); // فعال کردن بافت
    }
    glActiveTexture(GL_TEXTURE0); // همیشه Texture Unit 0 را به حالت پیش‌فرض برگردان

    // رسم مش
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}