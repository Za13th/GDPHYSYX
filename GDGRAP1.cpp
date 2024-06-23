#pragma once

#include <list>




void Polar(float r, float theta, float* x, float* y, float originX, float originY) //converting cartesian to polar coordinates
{//radius, theta in degress, and the resulting "x" and "y" values depending on the axis of rotation
    theta *= 3.14 / 180;
    *x = originX + (r * cos(theta));//originX and originY shift the point being rotated around their respective values
    *y = originY + (r * sin(theta));//or basically, they move the origin of the polar coordinates
}


class Light //Base Light Class
{
protected: //necessary data for all children of this class
    glm::vec3 lightColor;
    float ambientStr;
    glm::vec3 ambientColor;
    float specStr;
    float specPhong;
    float intensity;

public: 
    Light() //constructor, default values for both types of lights are set here
    { 
        this->lightColor = glm::vec3(1, 1, 1);
        this->ambientStr = 0.1f;
        this->ambientColor = lightColor;
        this->specStr = 0.5f;
        this->specPhong = 16;
    }

    void glUniformLight(glm::vec3 cameraPos, GLuint* shaderProg) //attaches data to the shaders
    {
        GLuint lightColorAddress = glGetUniformLocation(*shaderProg, "lightColor");
        glUniform3fv(lightColorAddress, 1, glm::value_ptr(this->lightColor));
        GLuint ambientStrAddress = glGetUniformLocation(*shaderProg, "ambientStr");
        glUniform1f(ambientStrAddress, this->ambientStr);
        GLuint ambientColorAddress = glGetUniformLocation(*shaderProg, "ambientColor");
        glUniform3fv(ambientColorAddress, 1, glm::value_ptr(this->ambientColor));
        GLuint  cameraPosAddress = glGetUniformLocation(*shaderProg, "cameraPos");
        glUniform3fv(cameraPosAddress, 1, glm::value_ptr(cameraPos));
        GLuint specStrAddress = glGetUniformLocation(*shaderProg, "specStr");
        glUniform1f(specStrAddress, this->specStr);
        GLuint specPhongAddress = glGetUniformLocation(*shaderProg, "specPhong");
        glUniform1f(specPhongAddress, this->specPhong);
    }

    virtual void glUniformLight2(GLuint* shaderProg)//attaches children only data to the shaders
    {
        std::cout << "This is meant to be overriden below by Light's children.";
    }

};

class DirectionLight : public Light //Direction Light Class
{
private: glm::vec3 dirLight; //contains a vector representing the direction of the light


public:
    DirectionLight(): Light() //constructor using the base class's constructor to supplement initialization
    {
        this->intensity = 0.75f;
        this->dirLight = glm::vec3(0, 1, 0);//position is slightly above the ocean, but close enough 
    }
    void glUniformLight2(GLuint* shaderProg) //passes the intensity and direction of the light
    {
        GLuint dirLightAddress = glGetUniformLocation(*shaderProg, "dirLight");
        glUniform3fv(dirLightAddress, 1, glm::value_ptr(this->dirLight));
        GLuint intensityAddress = glGetUniformLocation(*shaderProg, "dirIntensity");
        glUniform1f(intensityAddress, this->intensity);
    }
};

class PointLight : public Light //Point Light Class, base for First and Third Person Cameras
{
private: float constant, linear, quadratic; //contains the components for attenuation
         glm::vec3 lightPos; //Position of the light
         bool release;//to avoid the intensity from switching at supersonic speeds

public:
    PointLight() : Light() //constructor using the base class's constructor to supplement initialization
    {
        this->lightPos = glm::vec3(0, 0, 0);
        this->constant = 1;
        this->linear = 0.35;
        this->quadratic = 0.44;
        this->release = true;
        this->intensity = 1.5f;
    }
    void glUniformLight2(GLuint* shaderProg) //passes the light's intensity, position and attenuation parts
    {
        GLuint lightAddress = glGetUniformLocation(*shaderProg, "lightPos");
        glUniform3fv(lightAddress, 1, glm::value_ptr(this->lightPos));
        GLuint constantAddress = glGetUniformLocation(*shaderProg, "constant");
        glUniform1f(constantAddress, this->constant);
        GLuint linearAddress = glGetUniformLocation(*shaderProg, "linear");
        glUniform1f(linearAddress, this->linear);
        GLuint quadraticAddress = glGetUniformLocation(*shaderProg, "quadratic");
        glUniform1f(quadraticAddress, this->quadratic);
        GLuint intensityAddress = glGetUniformLocation(*shaderProg, "pointIntensity");
        glUniform1f(intensityAddress, this->intensity);
    }

    void cycleIntensity(GLFWwindow* window) //swaps light intensity from high to mid, mid to low, and low to high
    {
        if (glfwGetKey(window, GLFW_KEY_F) && (release == true))//checks if the button is only now being pressed
        {
            if (intensity == 1.5f)
                intensity = 0.55f;
            else if (intensity == 0.55f)
                intensity = 0.15f;
            else if (intensity == 0.15f)
                intensity = 1.5f;
            release = false;
        }//locks light intensity switching until the F is released
        else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
            release = true;
            
    }

    void setLightPos(glm::vec3 pos) //setter for light position (used for attaching it to the front of the submarine)
    {      
        this->lightPos = pos;
    }
};

class Camera //contains all necessay details about the camera
{
protected: float rotation, rotation2; 
           float thetaY, thetaX, distance;
         //rotation is around the x, rotation2 is around the y
protected: glm::vec3 cameraPos, Center; //vectors representing the camera's position and the center of the view
protected: bool firstMouse;
         float lastX, lastY;
         glm::vec3 WorldUp; //world up vector 
       //firstMouse is for checking the initial frame of rotating the view using the mouse
       //the floats lastX and lastY are there for rotating via mouse pointer
public:
    Camera()//constructor, sets all values to their default/initial state
    {
        firstMouse = true;
        rotation = 0.0f; 
        rotation2 = 0.0f;
        cameraPos = glm::vec3(0.0, 0.0f, 0.0f);
        Center = glm::vec3(0.0f, 0.0f, -10.0f);
        lastX = 0;
        lastY = 0;
        WorldUp = glm::vec3(0, 1.0f, 0);
        thetaY = 0;
        thetaX = 0;
    }

    void  resetMouse()//resets firstMouse to avoid a "jump" in camera view
    {
        firstMouse = true;
    }
    glm::vec3 getCameraPos()//returns the camera's position vector
    {
        return cameraPos;
    }

    glm::mat4 getRotationMatrix()//gets the rotation matrix depending on the camera's rotation-based point of view 
    {
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(this->rotation2), glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)));
        //gets the rotation around the y axis (horizontal)                                                         negative to "uninverse" movement after rotation
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(this->rotation), glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f))); 
        //gets the rotation around the x axis (vertical)
        return rotationMatrix;
    }


    virtual void PlayerControl(GLFWwindow* window)
    { //allows for moving the camera based on the arrow keys pressed

        bool input = false;

        if (glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_S))
        {
            input = true;
            if (glfwGetKey(window, GLFW_KEY_W))//moves up
            {
                this->thetaY += 0.0009;
            }
            if (glfwGetKey(window, GLFW_KEY_S))//moves down
            {
                this->thetaY -= 0.0009;
            }

            if (this->thetaY > 3.14 / 2)
                this->thetaY = 3.14 / 2;

            if (this->thetaY < -3.14 / 2)
                this->thetaY = -3.14 / 2;
        }

        if (glfwGetKey(window, GLFW_KEY_A) || glfwGetKey(window, GLFW_KEY_D))
        {
            input = true;


            if (glfwGetKey(window, GLFW_KEY_A))//moves up
                this->thetaX += 0.06;

            if (glfwGetKey(window, GLFW_KEY_D))//moves down
                this->thetaX -= 0.06;


        }
        if (input)
        {
            Polar(this->distance, thetaX+90, &this->cameraPos.x, &this->cameraPos.z, this->Center.x, this->Center.z);
            this->cameraPos.y = this->distance * sin(thetaY);
        }

    }


    
 

    glm::mat4 getViewMatrix() //returns the viewMatrix to be used by the 3D models
    {

        glm::mat4 cameraPositionMatrix = glm::mat4(1.0);
        cameraPositionMatrix = glm::translate(cameraPositionMatrix, this->cameraPos * -1.0f);
        //translates the cameraPositionMatrix based on cameraPos


        glm::vec3 F = glm::vec3(this->Center - this->cameraPos);
        //forward vector creation based on the Center and cameraPos

        F = glm::normalize(F);//normalizes F
        glm::vec3 R = glm::normalize(glm::cross(F, WorldUp));
        glm::vec3 U = glm::normalize(glm::cross(R, F));
        //creates the right and up vectors

        glm::mat4 cameraOrientation = glm::mat4(1.0f);

        cameraOrientation[0][0] = R.x;
        cameraOrientation[1][0] = R.y;
        cameraOrientation[2][0] = R.z;

        cameraOrientation[0][1] = U.x;
        cameraOrientation[1][1] = U.y;
        cameraOrientation[2][1] = U.z;

        cameraOrientation[0][2] = -F.x;
        cameraOrientation[1][2] = -F.y;
        cameraOrientation[2][2] = -F.z;
        //creates the camera orientation matrix from the right, up, and (negative) forward vectors

        cameraOrientation = glm::rotate(cameraOrientation, glm::radians(this->rotation), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
        cameraOrientation = glm::rotate(cameraOrientation, glm::radians(this->rotation2), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
        //rotates the camera's view based on the current rotation values


        glm::mat4 viewMatrix = (cameraOrientation * cameraPositionMatrix);
        return viewMatrix; //creates and returns the view matrix
    }

    virtual glm::mat4 getProjection()
    {
        std::cout << "This isn't supposed to be called. This is meant to be overriden by the childrens' versions below";
        return glm::mat4(4.2);
    };
};

class OrthoCamera :public Camera//orthographic camera class
{
private: 
    float edgeL, edgeR, edgeB, edgeT, edgeN, edgeF; //left, right, top, bottom, near and far;components for ortho projection

public:

    OrthoCamera() :Camera() //constructor
    {
        cameraPos = glm::vec3(0.0, 0.0f, 5.0f);//moves the camera into place
        Center = glm::vec3(0.0f, 0, 0.0f);
        this->distance = 5;
        //Center = glm::vec3(0.0f, cameraPos.y+3, 0.0f);
        //rotation = 90.0f;//makes it face downwards

        this->edgeL = -400.0f;
        this->edgeR = 400.0f;
        this->edgeB = -400.0f;
        this->edgeT = 400.0f;
        this->edgeN = -1000.0f;
        this->edgeF = 1000.0f;
    }

    void resetPos(glm::vec3 subPos) //moves the camera back to the beginning position
    {
        cameraPos = glm::vec3(0.0, 0.0f, 5.0f);
        Center = glm::vec3(0.0f, 0, 0.0f);
    }

    glm::mat4 getProjection()//gets orthographic projection
    {
        return glm::ortho(edgeL, edgeR, edgeB, edgeT, edgeN, edgeF);
    }
};

class PerspectiveCamera :public Camera //for moving the camera
{
protected:

    float FOV, width, height, near, far; //components for the perspective projection
    

public:

    PerspectiveCamera(float width, float height) :Camera() //constructor
    {   //camera default position and rotations
        cameraPos = glm::vec3(0.0, 0.0f, 800.0f);//moves the camera into place
        Center = glm::vec3(0.0f, 0, 0.0f);

        this->FOV = 53.5f;
        this->width = width;
        this->height = height;
        this->near = 0.1f;
        this->far = 2800.f;
        this->distance = 800;
    }
    
    
    glm::mat4 getProjection() //gets perspective projection
    {
        return glm::perspective(glm::radians(this->FOV), width / height, near, far);
    }
};



class Shader //Shader Class
{
private: GLuint shaderProg;//Shader Program
public: 
    Shader() = default;
    Shader(const char* vertPath, const char* fragPath)//Creates shader program based in the files givenm
    {
        GLuint vertexShader,fragShader;
        bool vNULL=true, fNULL=true;
        this->shaderProg = glCreateProgram();
        if (vertPath != NULL)//vertex shader creation
        {
            vNULL = false;
            std::fstream vertSrc(vertPath);
            std::stringstream vertBuff;
            vertBuff << vertSrc.rdbuf();

            std::string vertS = vertBuff.str();
            const char* v = vertS.c_str();
            vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &v, NULL);
            glCompileShader(vertexShader);
            glAttachShader(shaderProg, vertexShader);//attaches the vertex shader
        }

        if (fragPath != NULL) //fragment shader creation
        {
            fNULL = false;
            std::fstream fragSrc(fragPath);
            std::stringstream fragBuff;
            fragBuff << fragSrc.rdbuf();

            std::string fragS = fragBuff.str();
            const char* f = fragS.c_str();
            fragShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragShader, 1, &f, NULL);
            glCompileShader(fragShader);
            glAttachShader(shaderProg, fragShader);
        }
        
        //links the full shader
        glLinkProgram(shaderProg);

        //cleanup
        if (!vNULL)
        glDeleteShader(vertexShader);
        if(!fNULL)
        glDeleteShader(fragShader);
    }
    void useProgram()//tells the program to use this shader for the models below
    {
        glUseProgram(shaderProg);
    }

    GLuint* getProgram() //gets the shader program for drawing the models
    {
        return &this->shaderProg;
    }
};

class Model //Model Class
{
private: GLuint VAO,VBO; //VAO and VBO of the model
         std::vector<GLfloat> fullVertexData; //vertex data
         int size; //number of vertex attributes to account for

public:
    Model() = default;

    Model(const char* filepath) //for the submarine, or models which need to be normal mapped
    {
        std::string path = filepath; //get the model from the file path
        std::vector <tinyobj::shape_t> shapes;
        std::vector <tinyobj::material_t> material;
        std::string warning, error;

        tinyobj::attrib_t attributes;
        //load the model and the necessary information that comes with it
        bool success = tinyobj::LoadObj(&attributes, &shapes, &material, &warning, &error, path.c_str());
        //getting tangents and bitangents
        std::vector<glm::vec3> tangents, bitangents;
        
        tinyobj::index_t vData1, vData2, vData3;
        glm::vec3 v1, v2, v3;
        glm::vec2 uv1, uv2, uv3;
        glm::vec3 deltaPos1, deltaPos2;
        glm::vec2 deltaUV1, deltaUV2;
        float r;
        glm::vec3 tangent, bitangent;
        for (int i = 0; i < shapes[0].mesh.indices.size(); i += 3)
        {
            vData1 = shapes[0].mesh.indices[i];
            vData2 = shapes[0].mesh.indices[i+1];
            vData3 = shapes[0].mesh.indices[i+2];

            v1 = glm::vec3(attributes.vertices[vData1.vertex_index * 3],
                           attributes.vertices[(vData1.vertex_index * 3) + 1],
                           attributes.vertices[(vData1.vertex_index * 3) + 2]);

            v2 = glm::vec3(attributes.vertices[vData2.vertex_index * 3],
                attributes.vertices[(vData2.vertex_index * 3) + 1],
                attributes.vertices[(vData2.vertex_index * 3) + 2]);

            v3 = glm::vec3(attributes.vertices[vData3.vertex_index * 3],
                attributes.vertices[(vData3.vertex_index * 3) + 1],
                attributes.vertices[(vData3.vertex_index * 3) + 2]);

            uv1 = glm::vec2(attributes.texcoords[vData1.texcoord_index * 2], attributes.texcoords[(vData1.texcoord_index * 2)+1]);
            uv2 = glm::vec2(attributes.texcoords[vData2.texcoord_index * 2], attributes.texcoords[(vData2.texcoord_index * 2) + 1]);
            uv3= glm::vec2(attributes.texcoords[vData3.texcoord_index * 2], attributes.texcoords[(vData3.texcoord_index * 2) + 1]);

            deltaPos1 = v2 - v1;
            deltaPos2 = v3 - v1;

            deltaUV1 = uv2 - uv1;
            deltaUV2 = uv3 - uv1;

            r = 1.0f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));
            tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
            bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

            tangents.push_back(tangent);
            tangents.push_back(tangent);
            tangents.push_back(tangent);

            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
            bitangents.push_back(bitangent);
        }

        //gets the vertex data of the main model, while also pushing back the tangents and bitangents
        for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
            tinyobj::index_t vData = shapes[0].mesh.indices[i];

           this->fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3)]);
           this->fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 1]);
           this->fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 2]);

           this->fullVertexData.push_back(attributes.normals[(vData.normal_index * 3)]);
           this->fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 1]);
           this->fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 2]);

           this->fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2)]);
           this->fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2) + 1]);

           this->fullVertexData.push_back(tangents[i].x);
           this->fullVertexData.push_back(tangents[i].y);
           this->fullVertexData.push_back(tangents[i].z);

           this->fullVertexData.push_back(bitangents[i].x);
           this->fullVertexData.push_back(bitangents[i].y);
           this->fullVertexData.push_back(bitangents[i].z);
            
        }
        //initializes the VAO, VBO for the model
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);

        //Binds the VAO and VBO
        glBindVertexArray(this->VAO);
        glBindVertexArray(this->VBO);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->fullVertexData.size(), this->fullVertexData.data(), GL_DYNAMIC_DRAW);

            this->size = 14;// 3 from the vertices, 3 from the normals, 2 from the textures, and 6 from the tangents and bitangents

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, this->size * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        GLintptr normalPtr = 3 * sizeof(float);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, this->size * sizeof(float), (void*)normalPtr);
            glEnableVertexAttribArray(1);
        
        GLintptr texcoordPtr = 6 * sizeof(float);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, this->size * sizeof(float), (void*)texcoordPtr);
            glEnableVertexAttribArray(2);

       GLintptr tangentPtr = 8 * sizeof(float);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, this->size * sizeof(float), (void*)tangentPtr);
            glEnableVertexAttribArray(3);

       GLintptr bitangentPtr = 11 * sizeof(float);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, this->size * sizeof(float), (void*)bitangentPtr);
            glEnableVertexAttribArray(4);
    }
    

    Model(const char* filepath, bool normals, bool textureCoords)
    {//same as above, without normal mapping and also allows for choosing to exclude texture coordinates and normals
        std::string path = filepath; 
        std::vector <tinyobj::shape_t> shapes;
        std::vector <tinyobj::material_t> material;
        std::string warning, error;

        tinyobj::attrib_t attributes;

        bool success = tinyobj::LoadObj(&attributes, &shapes, &material, &warning, &error, path.c_str());

        //gets the vertex data of the main model
        for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
            tinyobj::index_t vData = shapes[0].mesh.indices[i];

                this->fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3)]);
                this->fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 1]);
                this->fullVertexData.push_back(attributes.vertices[(vData.vertex_index * 3) + 2]);
            
            if (normals==true) {
                this->fullVertexData.push_back(attributes.normals[(vData.normal_index * 3)]);
                this->fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 1]);
                this->fullVertexData.push_back(attributes.normals[(vData.normal_index * 3) + 2]);
            }
            if (textureCoords==true) {
                this->fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2)]);
                this->fullVertexData.push_back(attributes.texcoords[(vData.texcoord_index * 2) + 1]);
            }
        }
        //initializes the VAO, VBO for the model
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &this->VBO);
 
        //Binds the VAO and VBO
        glBindVertexArray(this->VAO);
        //Currently editing VAO = null
        glBindVertexArray(this->VBO);
        //Currently editing VBO = null

        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);//stores vertex data in array

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->fullVertexData.size(), this->fullVertexData.data(), GL_DYNAMIC_DRAW);
        

        if (normals == true && textureCoords == true)//changes size based on the parameters
            this->size = 8; 
        else if (normals == true)
            this->size = 6;
        else if (textureCoords == true)
            this->size = 5;
        else
            this->size = 3;


        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, this->size * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        //only occurs if the approriate boolean parameter is true
        GLintptr normalPtr = 3 * sizeof(float);
        if (normals == true) {
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, this->size * sizeof(float), (void*)normalPtr);
            glEnableVertexAttribArray(1);
        }

        GLintptr texcoordPtr = 6 * sizeof(float);
        if (textureCoords == true) {
            if(normals==false)
            texcoordPtr = 3 * sizeof(float);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, this->size * sizeof(float), (void*)texcoordPtr);
            glEnableVertexAttribArray(2);
        }  
        
    }

    GLuint* getVAO()//gets the VAO
    {
        return &this->VAO;
    }
    int getVertexDataSize()//gets the full vertex data size
    {
        return this->fullVertexData.size();
    }
    int getSize() //gets the number of full vertex data components to account for
    {
        return this->size;
    }
    void cleanUp()//deletes the VAO and VBO
    {
       glDeleteVertexArrays(1, &this->VAO);
       glDeleteBuffers(1, &this->VBO);
    }

};

class Entity
{
protected: float x, y, z; //translate
protected: float thetaX, thetaY, thetaZ; //rotate
protected: float sx, sy, sz;//scale
protected: Model model;//model to be associated with the entity

public: Entity() = default;//constructors
public: Entity(float x, float y, float z,
    float thetaX, float thetaY, float thetaZ,
    float sx, float sy, float sz, Model model)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->thetaX = thetaX;
    this->thetaY = thetaY;
    this->thetaZ = thetaZ;
    this->sx = sx;
    this->sy = sy;
    this->sz = sz;
    this->model = model;
}

      void draw(GLuint* shaderProg, Camera* cam)//draws the 3D Model
      {

          //gets the projection matrix from the camera
          glm::mat4 projection = cam->getProjection();
          //gets the view matrix from the camera
          glm::mat4 viewMatrix = cam->getViewMatrix();


          //creates the transformation matrix using the 3D models's attributes
          glm::mat4 transformation_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(this->x, this->y, this->z));
          transformation_matrix = glm::scale(transformation_matrix, glm::vec3(this->sx, this->sy, this->sz));
          transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaX), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
          transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaY), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
          transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaZ), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));

          
          //passes and recieves the necessary information to and from the shaders
          unsigned int viewLoc = glGetUniformLocation(*shaderProg, "view");
          glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

          unsigned int projLoc = glGetUniformLocation(*shaderProg, "projection");
          glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

          unsigned int transformLoc = glGetUniformLocation(*shaderProg, "transform");
          glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

          //Binds the VAO 
          glBindVertexArray(*this->model.getVAO());

          //draws the 3D model
          glDrawArrays(GL_TRIANGLES, 0, this->model.getVertexDataSize() / this->model.getSize());

      }


      void draw(GLuint* shaderProg,Camera* cam, GLuint* texture, GLuint* normalTex ,std::vector<Light*> Lights)//draws the 3D Model
      {

          //gets the projection matrix from the camera
          glm::mat4 projection = cam->getProjection();
          //gets the view matrix from the camera
          glm::mat4 viewMatrix = cam->getViewMatrix();


          //creates the transformation matrix using the 3D models's attributes
          glm::mat4 transformation_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(this->x, this->y, this->z));
          transformation_matrix = glm::scale(transformation_matrix, glm::vec3(this->sx, this->sy, this->sz));
          transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaX), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
          transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaY), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
          transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaZ), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));

          glActiveTexture(GL_TEXTURE0);
          GLuint tex0address = glGetUniformLocation(*shaderProg, "tex0");//texture related stuff
          glBindTexture(GL_TEXTURE_2D, *texture);
          glUniform1i(tex0address, 0);

          if (normalTex)//if normal mapped, 
          {
              glActiveTexture(GL_TEXTURE1);
              GLuint tex1address = glGetUniformLocation(*shaderProg, "norm_tex");//texture related stuff
              glBindTexture(GL_TEXTURE_2D, *normalTex);
              glUniform1i(tex1address, 1);
          }

          //passes and recieves the necessary information to and from the shaders
          unsigned int viewLoc = glGetUniformLocation(*shaderProg, "view");
          glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

          unsigned int projLoc = glGetUniformLocation(*shaderProg, "projection");
          glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

          unsigned int transformLoc = glGetUniformLocation(*shaderProg, "transform");
          glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

          //passes and recieves the necessary information to and from the shaders (lights)
          Lights[1]->glUniformLight(cam->getCameraPos(), shaderProg);

          Lights[0]->glUniformLight2(shaderProg);//direction light stuff
          Lights[1]->glUniformLight2(shaderProg);//point light stuff 

          //Binds the VAO 
          glBindVertexArray(*this->model.getVAO());

          //draws the 3D model
          glDrawArrays(GL_TRIANGLES, 0, this->model.getVertexDataSize() / this->model.getSize());

      }

      glm::vec3 getPos() //gets the entity's position
      {
          return glm::vec3(this->x, this->y, this->z);
      }

      void cleanUp() //calls the models clean up function to delete its VAO and VBO
      {
          this->model.cleanUp();
      }

};
    



class Player: public Entity //Player Class, derived from the entity
{

public: Player(float x, float y, float z,//constructor, that uses Entity's constructor as a base
    float thetaX, float thetaY, float thetaZ,
    float sx, float sy, float sz, Model model)
    : Entity( x,  y,  z,
         thetaX,  thetaY,  thetaZ,
        sx, sy, sz, model) {
}

      glm::mat4 getRotationMatrix()//gets the rotation matrix depending on the camera's rotation-based point of view 
      {
          glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(this->thetaY), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
          //gets the rotation around the y axis (horizontal)                                                        
          return rotationMatrix;
      }

      void PlayerControls(GLFWwindow* window)//for player controls
      {
          if (glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_S))
          {//moves forward/backwards based on the where the player is looking 
              glm::vec4 rotatedVector = this->getRotationMatrix() * glm::vec4(glm::vec3(0, 0, -0.01), 0.0f);
              //multiplies the direction of movement by the rotations
              glm::vec3 offset = glm::vec3(rotatedVector.x, rotatedVector.y, rotatedVector.z);
              //gets the offset in order to move forward/backward based on the way the camera is facing

              if (glfwGetKey(window, GLFW_KEY_W))//moves forward
              {
                  this->x += offset.x;
                  this->z += offset.z;
              }
              else //moves backward
              {
                  this->x -= offset.x;
                  this->z -= offset.z;
              }
          }

          if (glfwGetKey(window, GLFW_KEY_A)) //rotates the model left
          {
              this->thetaY += 0.1f;
          }
          else if (glfwGetKey(window, GLFW_KEY_D))//rotates the model right
          {
              this->thetaY -= 0.1f;
          }

          if ((glfwGetKey(window, GLFW_KEY_Q) || glfwGetKey(window, GLFW_KEY_E)))
          {
              if (glfwGetKey(window, GLFW_KEY_Q) && this->y <= 0)//moves the submarine up...
              {
                  this->y += 0.01;
                  if (this->y > 0) {//...unless it's at the top of the ocean, at which point, it can't go any further up
                      this->y = 0.0f;
                  }
              }
              else if (glfwGetKey(window, GLFW_KEY_E))//moves down
              {
                  this->y -= 0.01;
              }
          }
          std::cout << this->y << std::endl;//prints the current depth of the submarine
      }
};

class Texture {//for loading textures
protected: GLuint texture;//actual texture
public:
    Texture() = default;
    Texture(const char* filepath, bool alpha)
    {//loads the texture
        int img_width, img_height, colorChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* tex_bytes = stbi_load(filepath, &img_width, &img_height, &colorChannels, 0);
        glGenTextures(1, &this->texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->texture);

        if(alpha==true)//chooses between rgba and rgb based on the boolean parameter
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_bytes);
        else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_bytes);

        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(tex_bytes);//cleans up
    }
    GLuint* getTexture()// retrieves the texture
    {
        return &this->texture;
    }

};

class TextureNormal: public Texture//for loading the texture normal
{
public:
    TextureNormal() = default;
    TextureNormal(const char* filepath, bool alpha)//same as above, with changes 
    {
        int img_width, img_height, colorChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* tex_bytes = stbi_load(filepath, &img_width, &img_height, &colorChannels, 0);
        glGenTextures(1, &this->texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->texture);
        //changes here, because this texture is used for normal mapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        if (alpha == true)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_bytes);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_bytes);

        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(tex_bytes);
    }
};
class Filter : public Entity//filter used for the first person view
{
private: Camera* attachedCam;//for attaching the filter to the camera
       float rotation; //amount of rotation for polar movement
       void updatePos()//updates the position of the filter based on the location and rotation of the camera
       {
           y = this->attachedCam->getCameraPos().y;
           Polar(0.2, this->rotation, &this->x, &this->z, this->attachedCam->getCameraPos().x, this->attachedCam->getCameraPos().z);
       }
public: 
    Filter() = default;

    Filter(float x, float y, float z,
        float thetaX, float thetaY, float thetaZ,
        float sx, float sy, float sz, Model model)
        : Entity(x, y, z,
            thetaX, thetaY, thetaZ,
            sx, sy, sz, model) {}
    void attachCam(Camera* attachedCam)//attaches the filter to the camera
    {
        this->attachedCam = attachedCam;
        this->thetaY = 0;
        this->rotation = 270;
        this->updatePos();
    }
    void rotateCam(GLFWwindow* window) //horizontally rotates and moves the filter based on the camera
    {
        if (glfwGetKey(window, GLFW_KEY_A))//increments and decrements the related values based on the key pressed
        {
            thetaY += 0.1;
            rotation -= 0.1;
        }
        if (glfwGetKey(window, GLFW_KEY_D))
        {
            thetaY -= 0.1;
            rotation += 0.1;
        }
        this->updatePos();//updates filter position
    }

    glm::vec3 getCamFront()
    {//used for conveniently positioning the point light

        Polar(2.5f, this->rotation, &this->x, &this->z, this->attachedCam->getCameraPos().x, this->attachedCam->getCameraPos().z);
        //moves the filter to the position 2.5 ahead of where the submarine is (rotations accounted for)
        glm::vec3 camFront = this->getPos();//saves this position

        Polar(0.2f, this->rotation, &this->x, &this->z, this->attachedCam->getCameraPos().x, this->attachedCam->getCameraPos().z);
        //moves it back to where it originally was
        return camFront;//returns the saved position
    }
};

class ThirdPersonCamera : public PerspectiveCamera
{
private:
    Player* submarine;//attached submarine
    float thetaY;//for amount to rotate via polar movement
public: 

    ThirdPersonCamera(float width, float height, Player* submarine) : PerspectiveCamera(width, height)
    {//constructor
        this->firstMouse = true;
        this->submarine = submarine;//attaches the submarine
        this->Center = this->submarine->getPos();
        this->cameraPos = glm::vec3(0.0f, 0.0f, Center.z + 6);
        this->thetaY = 90;
        this->updatePos();
     
    }
    
    void updatePos()//updates the position of the camera based on the values changed by MouseControls below
    {
        //moves the camera to the appropriate position
        Polar(6, this->thetaY, &this->cameraPos.x, &this->cameraPos.z,
            this->submarine->getPos().x, this->submarine->getPos().z);
 
        this->cameraPos.y = this->submarine->getPos().y;//sets the camera to the same depth as the subs
        this->Center = submarine->getPos();//sets the center of the camera to the submarine
    }

    void MouseControls(GLFWwindow* window, double xpos, double ypos)
    {
        glfwGetCursorPos(window, &xpos, &ypos);
        //stores the x and y positions of the cursor in xpos, and ypos respectively
        if (this->firstMouse)
        {
            this->lastX = xpos;
            this->firstMouse = false;
        }//assignes the initial "last x and y values" of the mouse cursor to be the ones initially gotten to avoid a sudden jump in camera movement

        float offsetX = xpos - this->lastX;//gets the offset to move by based on horizontal movement
        this->lastX = xpos;
        
        offsetX *= 0.1f;//multiplies the offsets gotten earlier by 0.1 to control the camera movement so it doesn't
                         //go crazy

        this->thetaY += offsetX;//moves the camera around the submarine based on the offset later
         }
    };
class FPSCamera :public PerspectiveCamera
{
private:
    Filter* filter;//attached filter to the camera

public:

    FPSCamera(float width, float height,Filter* filter) :PerspectiveCamera(width,height) //constructor
    {
        this->filter = filter; //attaches the camera to the filter
        this->filter->attachCam(this);
    }

    Filter* getFilter()//returns the filter
    {
        return this->filter;
    }
    void PlayerControls(GLFWwindow* window)
    {//moves the camera alongside the player (same as player movement)
        if (glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_S))
        {
            glm::vec4 rotatedVector = getRotationMatrix() * glm::vec4(glm::vec3(0, 0, -0.01), 0.0f);
            //multiplies the direction of movement by the rotations
            glm::vec3 offset2 = glm::vec3(rotatedVector.x, rotatedVector.y, rotatedVector.z);
            if (glfwGetKey(window, GLFW_KEY_W))//moves forward
            {
                cameraPos += offset2;//moves both the camera and center by the offset
                Center += offset2;
            }
            else //moves backward
            {
                cameraPos -= offset2;
                Center -= offset2;
            }
            //gets the offset in order to move forward/backward based on the way the camera is facing
        }

        if (glfwGetKey(window, GLFW_KEY_A)) //rotates the camera left
        {
            this->rotation2 -= 0.1f;
        }
        else if (glfwGetKey(window, GLFW_KEY_D))//rotates the camera right
        {
            this->rotation2 += 0.1;
        }

        if ((glfwGetKey(window, GLFW_KEY_Q) || glfwGetKey(window, GLFW_KEY_E)))
        {
            if (glfwGetKey(window, GLFW_KEY_Q) && this->cameraPos.y <= 0)//moves up unless the ceiling depth of 0 is reached
            {
                cameraPos += glm::vec3(0, 0.01, 0);
                Center += glm::vec3(0, 0.01, 0);
                if (this->cameraPos.y > 0) {
                    this->cameraPos.y = 0.0f;
                    this->Center.y = 0.0f;
                }
            }
            else if (glfwGetKey(window, GLFW_KEY_E))//moves down
            {
                cameraPos -= glm::vec3(0, 0.01, 0);
                Center -= glm::vec3(0, 0.01, 0);
            }
        }
        this->filter->rotateCam(window);//rotates the filter based on the camera's orientation
    }
};
