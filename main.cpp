#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <cmath>
#include <ctime>

#include "Shader.h" // کلاس شیدر
#include "Model.h"  // کلاس مدل (که شامل Mesh هم هست)

// تنظیمات دوربین (می‌توانید تغییر دهید)
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 12.0f); // <-- زوم باز هم کمتر شد
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

int main() {
    // --- ۱. راه‌اندازی GLFW و ساخت پنجره ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "3D Chronograph Watch Model", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // --- ۲. راه‌اندازی GLEW ---
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST); // فعال کردن تست عمق

    // --- ۳. ساخت شیدرها ---
    Shader ourShader("vertex_shader.glsl", "fragment_shader.glsl");

    // --- ۴. بارگذاری مدل ساعت ---
    // مسیر به فایل gltf را بدهید. باید در پوشه watch_model باشد.
    Model ourModel("watch_model/scene.gltf"); 

    // --- حلقه اصلی برنامه ---
    while (!glfwWindowShouldClose(window)) {
        // --- Input ---
        // (اینجا می‌توانید ورودی کیبورد/ماوس را اضافه کنید)

        // --- Render ---
        glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // فعال کردن شیدر
        ourShader.use();

        // تنظیمات نور
        glm::vec3 lightPos(2.0f, 1.5f, 4.0f);
        ourShader.setVec3("lightPos", lightPos);
        ourShader.setVec3("viewPos", cameraPos);
        ourShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

        // ماتریس‌های تبدیل
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)800, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // ماتریس مدل: چرخش و مقیاس مدل
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f)); // مدل را کمی پایین‌تر ببر
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f)); // مدل را کوچک کن (معمولا مدل‌های وارداتی بزرگ هستند)
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // مدل را بچرخان

        ourShader.setMat4("model", model);
        ourShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f); // رنگ سفید (تا بافت‌ها نمایش داده شوند)

        // رسم مدل
        ourModel.Draw(ourShader);
        
        // --- Swap buffers ---
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- پاک‌سازی ---
    glfwTerminate();
    return 0;
}