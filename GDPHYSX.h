#pragma once

#include <cmath>
#include <cstdlib>
#include <time.h>
#include <glm/gtc/type_ptr.hpp>
#include "GDGRAP1.cpp"

namespace physics{

    class MyVector
    {
        public: float x,y,z;

        public: 
            MyVector();
            MyVector(const float _x, const float _y, const float _z);

            float Magnitude();
            MyVector Direction();

            MyVector operator+ (const int n);
            MyVector operator+ (const float f);
            MyVector operator+= (const int num);
            MyVector operator+= (const float f);

            MyVector operator+ (const MyVector v);
            MyVector operator+= (const MyVector v);

            MyVector operator- (const int n);
            MyVector operator- (const float f);
            MyVector operator-= (const int num);
            MyVector operator-= (const float f);

            MyVector operator- (const MyVector v);
            MyVector operator-= (const MyVector v);

            //scalar multiplication
            MyVector operator* (const int n);
            MyVector operator* (const float f);
            MyVector operator*= (const int num);
            MyVector operator*= (const float f);

            //component product
            MyVector operator* (const MyVector v);
            MyVector operator*= (const MyVector v);

            MyVector operator/ (const int n);
            MyVector operator/ (const float f);
            MyVector operator/= (const int num);
            MyVector operator/= (const float f);

            MyVector operator/ (const MyVector v);
            MyVector operator/= (const MyVector v);
            
            operator glm::vec3() const;
        
    };

    float dotProduct(MyVector A, MyVector B);
    MyVector crossProduct(MyVector A, MyVector B);

    class Particle

    {
    public: float  x, y, z, //translate
        scale,damping, mass, count; 
    public: int lifespan;
    public: physics::MyVector velocity, accel, color, accumulatedForce;
    public: Model* model;//model to be associated with the entity

    public: 
        Particle();//constructors
        Particle(Model* model);
        Particle(MyVector xyz, MyVector v, MyVector a, MyVector c, Model* model);

        void update(float time);
        bool destroy();
        void draw(GLuint* shaderProg, Camera* cam);
        void AddForce(MyVector force);
        void ResetForce();
        MyVector getPos();
        void cleanUp();

    };

    class ForceGenerator
    {
    public:
        virtual void UpdateForce(Particle* p, float time);
    };

    class GravityForceGenerator : public ForceGenerator
    {
    private: MyVector Gravity = MyVector(0, -9.8f, 0);

    public:
        GravityForceGenerator(const MyVector gravity);
        void UpdateForce(Particle* p, float time) override;
    };

    class DragForceGenerator : public ForceGenerator
    {
    private:
        float k1 = 0.74;
        float k2 = 0.57;

    public:
        DragForceGenerator();
        DragForceGenerator(float _k1, float _k2);
        void UpdateForce(Particle* p, float time) override;
    };

    class ForceRegistry
    {
    protected: struct ParticleForceRegistry
    {
        Particle* particle;
        ForceGenerator* generator;
    };
             std::list<ParticleForceRegistry> Registry;

    public:
        void Add(Particle* particle, ForceGenerator* generator);
        void Remove(Particle* particle, ForceGenerator* generator);
        void Clear();
        void UpdateForces(float time);
    };

    class PhysicsWorld
    {
    public: std::list<Particle*> Particles;
          ForceRegistry forceRegistry;
          //the two below are mainly for the MP (internal particle generation inside UpdateParticleList())
          Model* model; 
          DragForceGenerator* drag;
    public: 
        PhysicsWorld();
        PhysicsWorld(Model* m, DragForceGenerator* d);
        void AddParticle(Particle* toAdd);
        void Draw(GLuint* shaderProg, Camera* cam);
        void Update(float Time);

    private:
        GravityForceGenerator Gravity = GravityForceGenerator(MyVector(0.f, -9.8f, 0.f));
        void UpdateParticleList();
    };
}