#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader
{
public:
    unsigned int ID; // شناسه برنامه شیدر در OpenGL
    
    // سازنده: شیدر را از فایل‌ها بارگذاری و کامپایل می‌کند
    Shader(const char* vertexPath, const char* fragmentPath);
    
    // فعال کردن شیدر برای استفاده
    void use();
    
    // توابع کمکی برای ارسال مقادیر به یونیفرم‌های شیدر
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    // تابع کمکی برای بررسی خطاهای کامپایل/لینک شیدر
    void checkCompileErrors(unsigned int shader, std::string type);
};