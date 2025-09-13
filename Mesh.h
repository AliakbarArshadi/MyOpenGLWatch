#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h" // نیاز به کلاس Shader

// ساختار Vertex برای ذخیره مختصات موقعیت، نرمال و مختصات بافت
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

// ساختار Texture برای ذخیره شناسه بافت، نوع آن (مثلاً diffuse) و مسیر فایل
struct Texture {
    unsigned int id;
    std::string type; // مثل "texture_diffuse" یا "texture_specular"
    std::string path; // مسیر اصلی فایل بافت
};

// کلاس Mesh: نمایانگر یک بخش از مدل سه بعدی با هندسه و بافت‌های خاص خود
class Mesh {
public:
    // داده‌های هندسی و بافتی
    std::vector<Vertex>       vertices; // لیست رئوس
    std::vector<unsigned int> indices;  // لیست ایندکس‌ها برای رسم بهینه‌تر
    std::vector<Texture>      textures; // لیست بافت‌های مربوط به این مش

    unsigned int VAO; // شناسه Vertex Array Object برای این مش

    // سازنده: مش را با داده‌های ورتکس، ایندکس و بافت مقداردهی می‌کند
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    // تابع برای رسم مش با استفاده از شیدر داده شده
    void Draw(Shader &shader);

private:
    // شناسه Vertex Buffer Object و Element Buffer Object
    unsigned int VBO, EBO;

    // تابع کمکی برای راه‌اندازی بافرهای OpenGL
    void setupMesh();
};