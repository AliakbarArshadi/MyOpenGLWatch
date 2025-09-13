#pragma once

#include <vector>
#include <string>
#include <map> // برای ذخیره بافت‌های لود شده

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>      // کلاس اصلی Assimp برای وارد کردن مدل
#include <assimp/scene.h>           // ساختار داده برای صحنه مدل
#include <assimp/postprocess.h>     // گزینه‌های پردازش مدل

#include "Mesh.h" // نیاز به کلاس Mesh
// #include "stb_image.h" // در Model.cpp تعریف می‌شود


// تابع برای بارگذاری بافت از فایل
unsigned int TextureFromFile(const char *path, const std::string &directory);

// کلاس Model: نمایانگر یک مدل سه بعدی کامل که از چندین Mesh تشکیل شده است
class Model 
{
public:
    // داده‌های مدل
    std::vector<Mesh> meshes;        // لیست تمام مش‌های تشکیل دهنده مدل
    std::string directory;           // مسیر دایرکتوری که مدل از آن لود شده
    std::vector<Texture> textures_loaded; // وکتور بافت‌های لود شده برای جلوگیری از لود مجدد

    // سازنده: مدل را از مسیر فایل مشخص شده بارگذاری می‌کند
    Model(std::string const &path);

    // تابع برای رسم کل مدل
    void Draw(Shader &shader);

private:
    // تابع اصلی برای بارگذاری مدل با استفاده از Assimp
    void loadModel(std::string const &path);
    // تابع برای پردازش نودهای (گره‌های) صحنه (بازگشتی)
    void processNode(aiNode *node, const aiScene *scene);
    // تابع برای پردازش یک مش Assimp و تبدیل آن به یک Mesh ما
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    // تابع برای بارگذاری بافت‌های یک متریال (ماده)
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};