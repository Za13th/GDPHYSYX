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
            MyVector(const float a);
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
        scale,radius,damping, mass, count, restitution; 
          //scale = radius
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
        void setPos(float x, float y, float z);
        void setPos(MyVector xyz);
        void cleanUp();

    };

    class ParticleContact
    {

    public: Particle* particles[2];
          float restitution;
          MyVector contactNormal;
          float depth;

          void Resolve(float time);
          float GetSeparatingSpeed();

    protected:
        void ResolveVelocity(float time);
        void ResolveInterpenetration(float time);


    };

    class ContactResolver {
    public: unsigned max_iterations;
          ContactResolver(unsigned max_iterations) : max_iterations(max_iterations){}
          void ResolveContacts(std::vector<ParticleContact*> contacts, float time);

    protected: unsigned current_iterations = 0;
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

    class AnchoredSpring : public ForceGenerator
    { 
        public: 
            MyVector anchorPoint;
            float springConstant, restLength;

            AnchoredSpring(MyVector anchorPoint, float _springConstant, float _restLength):
                anchorPoint(anchorPoint),springConstant(_springConstant),restLength(_restLength){}

            void UpdateForce(Particle* p, float time) override;

    };

    class ParticleSpring : public ForceGenerator
    {
    public:
        Particle* otherParticle;
        float springConstant, restLength;

        ParticleSpring(Particle* _particle, float _springConstant, float _restLength) :
            otherParticle(_particle), springConstant(_springConstant), restLength(_restLength) {}

        void UpdateForce(Particle* p, float time) override;

    };

    class ParticleLink
    {
          public: Particle* particles[2];
          virtual ParticleContact* GetContact() { return nullptr; }

    protected:
        float CurrentLength();
    };

    class Rod : public ParticleLink
    {
    public:
        float length = 1;
        float restitution = 0;

        ParticleContact* GetContact() override;
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


    class RenderLine
    {
    private:
        MyVector p1, p2;


    public:
        RenderLine(MyVector _p1, MyVector _p2) :
            p1(_p1), p2(_p2){}

        void Update(MyVector _p1, MyVector _p2);
        void Draw(glm::mat4 projectionMatrix);
    };
    class PhysicsWorld
    {
    public: std::list<Particle*> Particles;
          ForceRegistry forceRegistry;
          std::vector<ParticleContact*> Contacts;
          ContactResolver Resolver = ContactResolver(20);
          //the two below are mainly for the MP (internal particle generation inside UpdateParticleList())
          Model* model; 
          DragForceGenerator* drag;

          std::list<ParticleLink*> Links;
    public: 
        PhysicsWorld();
        PhysicsWorld(float gravityForce);
        PhysicsWorld(Model* m, DragForceGenerator* d);
        void AddParticle(Particle* toAdd);
        void AddContact(Particle* p1, Particle* p2, float restitution, MyVector contactNormal,float depth);
        void Draw(GLuint* shaderProg, Camera* cam);
        void Update(float Time);
        void GetOverlaps();
        

    private:
        GravityForceGenerator Gravity = GravityForceGenerator(MyVector(0.f, -9.8f, 0.f));
        //GravityForceGenerator Gravity = GravityForceGenerator(MyVector(0.f, 0.f, 0.f));
        void UpdateParticleList();

    protected:
        void GenerateContacts();
    };


    


}