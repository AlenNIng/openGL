#include <iostream>
#define GLEW_STATIC
#include "GL/glew.h"
#include "glfw3.h"
#include "SOIL.h"


// GLM Mathematics
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#define SHADER_SOURCE(...) #__VA_ARGS__

const GLuint WIDTH = 800, HEIGHT = 600;
// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 normal;\n"
"layout (location = 2) in vec2 texCoord;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec2 TexCoord;\n"
"out vec3 Normal;\n"
"out vec3 FragPos;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * model *vec4(position.x, position.y, position.z, 1.0);\n"
"Normal = normal;\n"
" vec4 modelViewPos = model *vec4(position.x, position.y, position.z, 1.0);\n"
"FragPos = modelViewPos.xyz;\n"
"TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);\n"
"}\0";


const GLchar* fragmentShaderSource = "#version 330 core\n"
"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"
"out vec4 color;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"color = texture(ourTexture, TexCoord);\n"
"}\n\0";

GLuint createFBO()
{
    GLuint imageFBO = 0;
    glGenFramebuffers(1, &imageFBO);
    return imageFBO;
}

bool releaseFBO( GLuint fboID)
{
    if (0 == fboID)
    {
        return false;
    }
    glDeleteFramebuffers(1, &fboID);
    return true;
}

GLuint createTexture()
{
    GLuint textureID = 0;
    return textureID;
}

void releaseTexture(GLuint textureID)
{
    if (0 != textureID)
    {
        glDeleteTextures(1, &textureID);
    }
}

bool bindTextureToFBO(GLuint fboID, GLuint textureID)
{
    return true;
}

void offScreenRender()
{

}

void screenRender()
{

}

bool copyTextureToMemery(GLuint fboID, GLuint textureID, GLuint target,int width, int heigth)
{
    //
    // read out piexl
    //
    GLint oldFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    unsigned char* dstData = new unsigned char[width * heigth * 4];
    memset(dstData, 0, width * heigth * 4);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, textureID, 0);
    glReadPixels(0, 0, 800, 600, GL_RGBA, GL_UNSIGNED_BYTE, dstData);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        return false;
    }
    int save_result = SOIL_save_image
        (
        "opengltest1.png",
        SOIL_SAVE_TYPE_BMP,
        800, 600, 4,
        dstData
        );
    SOIL_free_image_data(dstData);
    glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
    return true;
}

#define VAOUSE 1
int main()
{
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Load image, create texture and generate mipmaps
    int w, h;
    unsigned char* image = SOIL_load_image("container.jpg", &w, &h, 0, SOIL_LOAD_RGBA);
    unsigned char * imageCopyData = new unsigned char[w * h * 4];
    memcpy(imageCopyData, image, w*h * 4);
    int save_result = SOIL_save_image
        (
        "opengltest1.png",
        SOIL_SAVE_TYPE_BMP,
        w, h, 4,
        imageCopyData
        );


    GLuint _quadVbo;
    GLuint _quadVao;
    GLuint _quadIbo;
    GLuint _copyProgram;
    GLuint _fbo;

    const char* v = R"(
attribute vec2 aPos;
attribute vec2 aUV;
varying vec2 vUV;
void main()
{
gl_Position = vec4(aPos, 0.0, 1.0);
vUV = aUV;
}
)";
    const char* f = R"(
precision mediump float;
uniform sampler2D uTexture;
varying vec2 vUV;
void main()
{
gl_FragColor = texture2D(uTexture, vUV);
}
)";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &v, NULL);
    glCompileShader(vs);

    int len = 0;
    glGetShaderInfoLog(vs, 0, &len, nullptr);
    if (len > 0)
    {
        std::string str;
        str.resize(len + 1);
        glGetShaderInfoLog(vs, str.size(), &len, &str[0]);
        printf("%s\n", str.c_str());
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &f, NULL);
    glCompileShader(fs);

    len = 0;
    glGetShaderInfoLog(fs, 0, &len, nullptr);
    if (len > 0)
    {
        std::string str;
        str.resize(len + 1);
        glGetShaderInfoLog(fs, str.size(), &len, &str[0]);
        printf("%s\n", str.c_str());
    }

    _copyProgram = glCreateProgram();
    glAttachShader(_copyProgram, vs);
    glAttachShader(_copyProgram, fs);
    glLinkProgram(_copyProgram);
    auto errorCode = glGetError();
    glDeleteShader(vs);
    glDeleteShader(fs);

    float vertices[] = {
        -1, 1, 0, 1,
        -1, -1, 0, 0,
        1, -1, 1, 0,
        1, 1, 1, 1
    };


    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); in this version of openGL and GLFW_OPENGL_CORE_PROFILE mode VAO must be one, if no VAO, glVertexAttribPointer raising GL_INVALID_OPERATION error

    glGenVertexArrays(1, &_quadVao);
    glBindVertexArray(_quadVao);

    glGenBuffers(1, &_quadVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _quadVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned short indices[] = {
        0, 1, 2, 0, 2, 3
    };
    glGenBuffers(1, &_quadIbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _quadIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

#if VAOUSE
    
    int locPos = glGetAttribLocation(_copyProgram, "aPos");
    glEnableVertexAttribArray(locPos);
    glVertexAttribPointer(locPos, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glDisableVertexAttribArray(locPos);

    int locUV = glGetAttribLocation(_copyProgram, "aUV");
    glEnableVertexAttribArray(locUV);
    glVertexAttribPointer(locUV, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));
    glDisableVertexAttribArray(locUV);
    
#endif
     
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    
    glGenFramebuffers(1, &_fbo);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageCopyData);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint textureout;
    glGenTextures(1, &textureout);
    glBindTexture(GL_TEXTURE_2D, textureout); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glBindTexture(GL_TEXTURE_2D, 0);

   
    int i = 1;
    while (!glfwWindowShouldClose(window)  && i<200)
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        auto err = glGetError();
        if (err != 0)
        {
            printf("DrawQuad glDraw error %d\n", err);
        }
        
        
        /*
        {
            GLint oldFBO;
            bool ret = true;
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);

            glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureout, 0);
            glViewport(0, 0, w, h);
        }
        */
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);


        int loc;
        glUseProgram(_copyProgram);
        GLboolean  ispro = glIsProgram(_copyProgram);
       
        loc = glGetUniformLocation(_copyProgram, "uTexture");
        glUniform1i(loc, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // You don't need a VAO in a compatibility context, but you do in a core context. Which you asked for. So... you need one:
        // https://stackoverflow.com/questions/13403807/glvertexattribpointer-raising-gl-invalid-operation
        // if no VAO the glEnableVertexAttribArray get GL error 1282
#if VAOUSE

        glBindVertexArray(_quadVao);
       
        glEnableVertexAttribArray(locPos);
        glEnableVertexAttribArray(locUV);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

        glDisableVertexAttribArray(locPos);
        glDisableVertexAttribArray(locUV);     
        glBindVertexArray(0);
#else
        
        glBindVertexArray(_quadVao);
        glBindBuffer(GL_ARRAY_BUFFER, _quadVbo);
        int locPos = glGetAttribLocation(_copyProgram, "aPos");
        glEnableVertexAttribArray(locPos);
        err = glGetError();
        glVertexAttribPointer(locPos, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        err = glGetError();

        int locUV = glGetAttribLocation(_copyProgram, "aUV");
        glEnableVertexAttribArray(locUV);
        glVertexAttribPointer(locUV, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _quadIbo);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

        err = glGetError();

        glDisableVertexAttribArray(locPos);
        glDisableVertexAttribArray(locUV);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#endif

        // Swap the screen buffers
        glfwSwapBuffers(window);

        /*
        {

            err = glGetError();
            if (err != 0)
            {
                printf("DrawQuad glDraw error %d\n", err);
            }

            glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureout, 0);
            glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, imageCopyData);
            glFinish();
            err = glGetError();
            if (err != GL_NO_ERROR)
            {
                return false;
            }
            char fileName[256];

            sprintf_s(fileName, "test%d.png", i);
            int save_result = SOIL_save_image
                (
                fileName,
                SOIL_SAVE_TYPE_BMP,
                w, h, 4,
                imageCopyData
                );

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
            i++;
        }
        */

    }

    SOIL_free_image_data(image);
    delete[] imageCopyData;
    return 0;


}

int main1()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);

	// Build and compile our shader program
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	// Set up vertex data (and buffer(s)) and attribute pointers
	//GLfloat vertices[] = {
	//	//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
	//	0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // 右上
	//	0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // 右下
	//	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   // 左下
	//	-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f    // 左上
	//};

    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,//1
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,//2
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,//3
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,//3
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,//4
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,//1    //背面

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,//5
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,//6
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,//7
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,//7
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,//8
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,//5     //正面

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,//8
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,//4
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,//1
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,//1
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,//5
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,//8     //左面

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,//7
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,//3
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,//2
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,//2
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,//6
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,//7     //右面

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,//1
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,//2
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,//6
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,//6
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,//5
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,//1    //底面

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,//4
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,//3
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,//7
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,//7
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,//8
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f//4      //上面
    };

	//GLuint indices[] = {  // Note that we start from 0!
	//	0, 1, 3, // First Triangle
	//	1, 2, 3  // Second Triangle
	//};
    GLuint VBO, VAO;//, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //glVertexAttribPointer explain how to use those data
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	/*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);*/
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO


	// Load and create a texture 
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// Set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load image, create texture and generate mipmaps
	int w, h;
	unsigned char* image = SOIL_load_image("container.jpg", &w, &h, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

    
    GLuint offscreenTexture;
    // create testure
    glGenTextures(1, &offscreenTexture);
    glBindTexture(GL_TEXTURE_2D, offscreenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    //
    //FBO 
    GLuint imageFBO;
    glGenFramebuffers(1, &imageFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, imageFBO);
    //attach texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, offscreenTexture, 0); 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };


	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
        //
        //bind FBO 
        //glBindFramebuffer(GL_FRAMEBUFFER, imageFBO);
        //glBindTexture(GL_TEXTURE_2D, offscreenTexture);

		// Clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		//glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw our first triangle
		// Bind Texture
		
		glUseProgram(shaderProgram);
        //激活纹理单元，绑定纹理，设置采样器对应的纹理单元， 激活单元跟设置单元要对应，不然采样出错
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
        
        // Create transformations
        //glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        //model = glm::rotate(model, (GLfloat)glfwGetTime() * 45.0f, glm::vec3(0.5f, 1.0f, 0.0f));
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -2.0f));
        GLfloat radius = 3.0f;
        GLfloat camX = (float)sin(glfwGetTime()) * radius;
        GLfloat camZ = (float)cos(glfwGetTime()) * radius;
        view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
        projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
        int size = sizeof(glm::mat4) / sizeof(float);
   
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
        // Pass them to the shaders
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        //glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);

		glBindVertexArray(VAO);//如果说VAO是VBO的集合，绘制时，绑定VAO就可以绘制，那么EBO是何时绑定的？
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//EBO在这个时候实现绑定
        for (GLuint i = 0; i < 10; i++)
        {
            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            GLfloat angle = 20.0f * (i +1);
            model = glm::rotate(model, (GLfloat)glfwGetTime() *angle, glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
    glDeleteTextures(1, &texture);
    glDeleteTextures(1, &offscreenTexture);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &imageFBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}


/*
GLuint _quadVao;
GLuint _quadVbo;
GLuint _quadIbo;
GLuint _copyProgram;
GLuint _fbo;
GLuint _tex_in;
GLuint _tex_out;
int _width = 0;
int _height = 0;

void initQuad()
{
float vertices[] = {
-1, 1, 0, 0,
-1, -1, 0, 1,
1, -1, 1, 1,
1, 1, 1, 0
};

glGenVertexArrays(1, &_quadVao);
glBindVertexArray(_quadVao);

glGenBuffers(1, &_quadVbo);
glBindBuffer(GL_ARRAY_BUFFER, _quadVbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

unsigned short indices[] = {
0, 1, 2, 0, 2, 3
};
glGenBuffers(1, &_quadIbo);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _quadIbo);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
glGenFramebuffers(1, &_fbo);
auto errorCode = glGetError();

}

void initCopyProgram()
{
const char* v = R"(
uniform mat4 uMat;
attribute vec2 aPos;
attribute vec2 aUV;
varying vec2 vUV;
void main()
{
gl_Position = uMat * vec4(aPos, 0.0, 1.0);
vUV = aUV;
}
)";
const char* f = R"(
precision mediump float;
uniform sampler2D uTexture;
varying vec2 vUV;
void main()
{
gl_FragColor = texture2D(uTexture, vUV);
}
)";

GLuint vs = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vs, 1, &v, NULL);
glCompileShader(vs);

int len = 0;
glGetShaderInfoLog(vs, 0, &len, nullptr);
if (len > 0)
{
std::string str;
str.resize(len + 1);
glGetShaderInfoLog(vs, str.size(), &len, &str[0]);
printf("%s\n", str.c_str());
}

GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(fs, 1, &f, NULL);
glCompileShader(fs);

len = 0;
glGetShaderInfoLog(fs, 0, &len, nullptr);
if (len > 0)
{
std::string str;
str.resize(len + 1);
glGetShaderInfoLog(fs, str.size(), &len, &str[0]);
printf("%s\n", str.c_str());
}

_copyProgram = glCreateProgram();
glAttachShader(_copyProgram, vs);
glAttachShader(_copyProgram, fs);
glLinkProgram(_copyProgram);
auto errorCode = glGetError();
glDeleteShader(vs);
glDeleteShader(fs);
}

void drawQuad(GLuint intex, GLuint outtex, int width, int height, float* transform)
{
int err = glGetError();

GLint oldFBO;
bool ret = true;
glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFBO);

glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outtex, 0);
glViewport(0, 0, width, height);
glClearColor(1, 0, 0, 1);
glClear(GL_COLOR_BUFFER_BIT);

int loc;
glUseProgram(_copyProgram);
GLboolean  ispro = glIsProgram(_copyProgram);
glActiveTexture(GL_TEXTURE0);
loc = glGetUniformLocation(_copyProgram, "uTexture");
glUniform1i(loc, 0);
glBindTexture(GL_TEXTURE_2D, intex);

loc = glGetUniformLocation(_copyProgram, "uMat");
glUniformMatrix4fv(loc, 1, false, transform);

glBindBuffer(GL_ARRAY_BUFFER, _quadVbo);

int locPos = glGetAttribLocation(_copyProgram, "aPos");
glEnableVertexAttribArray(locPos);
glVertexAttribPointer(locPos, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

int locUV = glGetAttribLocation(_copyProgram, "aUV");
glEnableVertexAttribArray(locUV);
glVertexAttribPointer(locUV, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));

glDisable(GL_CULL_FACE);
glDisable(GL_DEPTH_TEST);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _quadIbo);
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

glDisableVertexAttribArray(locPos);
glDisableVertexAttribArray(locUV);

glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
glBindFramebuffer(GL_FRAMEBUFFER, oldFBO);
err = glGetError();
if (err != 0)
{
printf("DrawQuad glDraw error %d\n", err);
}
}

void updateTexture(unsigned char *frame, int width, int height)
{
GLint last_texture;
auto errorCode = glGetError();
glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);

if (0 == _tex_in || 0 == _tex_out || width != _width || height != _height)
{
if (_tex_in)
{
glDeleteTextures(1, &_tex_in);
_tex_in = 0;
}

if (_tex_out)
{
glDeleteTextures(1, &_tex_in);
_tex_in = 0;
}

glGenTextures(1, &_tex_in);
glBindTexture(GL_TEXTURE_2D, _tex_in);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame);

glGenTextures(1, &_tex_out);
glBindTexture(GL_TEXTURE_2D, _tex_out);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

_width = width;
_height = height;
}
else
{
glBindTexture(GL_TEXTURE_2D, _tex_in);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame);
}
errorCode = glGetError();
// Restore state
glBindTexture(GL_TEXTURE_2D, last_texture);
errorCode = glGetError();
}

int main()
{
glfwInit();
// Set all the required options for GLFW
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

// Create a GLFWwindow object that we can use for GLFW's functions
GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
glfwMakeContextCurrent(window);

// Set the required callback functions
glfwSetKeyCallback(window, key_callback);

// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
glewExperimental = GL_TRUE;
// Initialize GLEW to setup the OpenGL Function pointers
if (glewInit() != GLEW_OK)
{
std::cout << "Failed to initialize GLEW" << std::endl;
return -1;
}
int w = 0;
int h = 0;
unsigned char* image = SOIL_load_image("container.jpg", &w, &h, 0, SOIL_LOAD_RGBA);
updateTexture(image, w, h);
SOIL_free_image_data(image);
initQuad();
initCopyProgram();
float identity[] = {
1.0f, 0.0f, 0.0f, 0.0f,
0.0f, 1.0f, 0.0f, 0.0f,
0.0f, 0.0f, 1.0f, 0.0f,
0.0f, 0.0f, 0.0f, 1.0f
};
float flipY[] = {
1.0f, 0.0f, 0.0f, 0.0f,
0.0f, -1.0f, 0.0f, 0.0f,
0.0f, 0.0f, 1.0f, 0.0f,
0.0f, 0.0f, 0.0f, 1.0f
};
drawQuad(_tex_in, _tex_out, w, h ,identity);

return 0;
}
*/