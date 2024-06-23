#include "GDPHYSX.h"

using namespace physics;

MyVector::MyVector() : x(0),y(0),z(0){}

MyVector::MyVector(const float _x, const float _y, const float _z): x(_x),y(_y),z(_z) {}

float MyVector::Magnitude()
{
    return sqrtf((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
}
MyVector MyVector::Direction()
{
    return MyVector(this->x/this->Magnitude(), this->y / this->Magnitude(), this->z / this->Magnitude());
}

MyVector MyVector::operator+ (const int n) { return MyVector(this->x + n, this->y + n, this->z + n); }
MyVector MyVector::operator+ (const float f) { return MyVector(this->x + f, this->y + f, this->z + f); }
MyVector MyVector::operator+= (const int num) { return MyVector(this->x += num, this->y += num, this->z += num); }
MyVector MyVector::operator+= (const float f) { return MyVector(this->x += f, this->y += f, this->z += f); }

MyVector MyVector::operator+ (const MyVector v) { return MyVector(this->x + v.x, this->y + v.y, this->z + v.z); }
MyVector MyVector::operator+= (const MyVector v) { return MyVector(this->x += v.x, this->y += v.y, this->z += v.z); }

MyVector MyVector::operator- (const int n) { return MyVector(this->x - n, this->y - n, this->z - n); }
MyVector MyVector::operator- (const float f) { return MyVector(this->x - f, this->y - f, this->z - f); }
MyVector MyVector::operator-= (const int num) { return MyVector(this->x -= num, this->y -= num, this->z -= num); }
MyVector MyVector::operator-= (const float f) { return MyVector(this->x -= f, this->y -= f, this->z -= f); }

MyVector MyVector::operator- (const MyVector v) { return MyVector(this->x - v.x, this->y - v.y, this->z - v.z); }
MyVector MyVector::operator-= (const MyVector v) { return MyVector(this->x -= v.x, this->y -= v.y, this->z -= v.z); }

            //scalar multiplication
MyVector MyVector::operator* (const int n) { return MyVector(this->x * n, this->y * n, this->z * n); }
MyVector MyVector::operator* (const float f) { return MyVector(this->x * f, this->y * f, this->z * f); }
MyVector MyVector::operator*= (const int num) { return MyVector(this->x *= num, this->y *= num, this->z *= num); }
MyVector MyVector::operator*= (const float f) { return MyVector(this->x *= f, this->y *= f, this->z *= f); }

            //component product
MyVector MyVector::operator* (const MyVector v) { return MyVector(this->x * v.x, this->y * v.y, this->z * v.z); }
MyVector MyVector::operator*= (const MyVector v) { return MyVector(this->x *= v.x, this->y *= v.y, this->z *= v.z); }

MyVector MyVector::operator/ (const int n) { return MyVector(this->x / n, this->y / n, this->z / n); }
MyVector MyVector::operator/ (const float f) { return MyVector(this->x / f, this->y / f, this->z / f); }
MyVector MyVector::operator/= (const int num) { return MyVector(this->x /= num, this->y /= num, this->z /= num); }
MyVector MyVector::operator/= (const float f) { return MyVector(this->x /= f, this->y /= f, this->z /= f); }

MyVector MyVector::operator/ (const MyVector v) { return MyVector(this->x / v.x, this->y / v.y, this->z / v.z); }
MyVector MyVector::operator/= (const MyVector v) { return MyVector(this->x /= v.x, this->y /= v.y, this->z /= v.z); }
            
MyVector::operator glm::vec3() const {return glm::vec3(x,y,z);}

float physics::dotProduct(MyVector A, MyVector B)
{
    return ((A.x * B.x) + (A.y * B.y) + (A.z * B.z));
}

MyVector physics::crossProduct(MyVector A, MyVector B)
{
    return MyVector
    (
        ((A.y * B.z) - (A.z * B.y)),
        ((A.z * B.x) - (A.x * B.z)),
        ((A.x * B.y) - (A.y * B.x))
    );
}



    Particle::Particle() = default;//constructors
    Particle::Particle(Model* model) //main constructor for sparks simulation
    {
        this->x = 0;
        this->y = -350; //near bottm of the screen
        this->z = 0;
        this->scale = rand() % 9 + 2; //2 - 10

        this->model = model;

        //generally upwards velocity with the possibilty to make it also go forwards/backward and left/right
        this->velocity = MyVector(rand() % 151 - 75, rand() % 191 + 105, rand() % 151 - 75);
        this->accel = MyVector(rand() % 41, 1, rand() % 41);
        //random color
        this->color = MyVector(0.01f * (rand() % 100 + 1), 0.01f * (rand() % 100 + 1), 0.01f * (rand() % 100 + 1));
        this->mass = 1;
        this->damping = 0.9f;
        this->accumulatedForce = MyVector(0, 0, 0);


        this->lifespan = rand() % 10 + 1; //1-10


        this->count = 0;//counter for lifespan
    }
    Particle::Particle(MyVector xyz, MyVector v, MyVector a, MyVector c, Model* model)
{


    this->x = xyz.x;
    this->y = xyz.y;
    this->z = xyz.z;
    this->scale = 10;

    this->model = model;

    this->velocity = v;
    this->accel = a;
    this->color = c;

    this->damping = 0.9f;
    this->accumulatedForce = MyVector(0, 0, 0);
    this->mass = 10;
    this->lifespan = 1000;


    this->count = 0;
}
    void Particle::update(float time)
      {

          //update position
          this->x += velocity.x * time + (0.5f * this->accel.x * time * time);
          this->y += velocity.y * time + (0.5f * this->accel.y * time * time);
          this->z += velocity.z * time + (0.5f * this->accel.z * time * time);

          //update velocity
          this->accel += accumulatedForce * (1 / this->mass);
          this->velocity += (accel * time);
          this->velocity *= powf(this->damping, time);

          this->ResetForce();

          count++;
          if (count >= 60) //assumes program is being run 60 frames per second
          {
              count = 0; //every 60 frames (or one second), decrease the lifespan by one
              this->lifespan--;
          }

      }
    bool Particle::destroy()
      {
          if (lifespan <= 0) //return true if the particle has reached the end of its lifespan
              return true;
          //else carry on as usual
          return false;
      }
    void Particle::draw(GLuint* shaderProg, Camera* cam)//draws the 3D Model
      {
          if (!destroy()) //draw only if not destroyed
          {
              //gets the projection matrix from the camera
              glm::mat4 projection = cam->getProjection();
              //gets the view matrix from the camera
              glm::mat4 viewMatrix = cam->getViewMatrix();


              //creates the transformation matrix using the 3D models's attributes
              glm::mat4 transformation_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(this->x, this->y, this->z));
              transformation_matrix = glm::scale(transformation_matrix, glm::vec3(this->scale));
              //transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaX), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
              //transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaY), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
              //transformation_matrix = glm::rotate(transformation_matrix, glm::radians(thetaZ), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));


              //passes and recieves the necessary information to and from the shaders
              glm::vec3 Color = glm::vec3(this->color.x, this->color.y, this->color.z);

              GLuint cLoc = glGetUniformLocation(*shaderProg, "color");
              glUniform3fv(cLoc, 1, glm::value_ptr(Color));

              unsigned int viewLoc = glGetUniformLocation(*shaderProg, "view");
              glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));

              unsigned int projLoc = glGetUniformLocation(*shaderProg, "projection");
              glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

              unsigned int transformLoc = glGetUniformLocation(*shaderProg, "transform");
              glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

              //Binds the VAO 
              glBindVertexArray(*this->model->getVAO());

              //draws the 3D model
              glDrawArrays(GL_TRIANGLES, 0, this->model->getVertexDataSize() / this->model->getSize());

          }
      }
    void Particle::AddForce(MyVector force)
      {  //adds the given force to be taken into account for the next update
          this->accumulatedForce += force;
      }
    void Particle::ResetForce()
      { //set force to zero
          this->accumulatedForce *= 0;
          this->accel *= 0;
      }
    MyVector Particle::getPos() //gets the entity's position
      {
          return MyVector(this->x, this->y, this->z);
      }
    void Particle::cleanUp() //calls the models clean up function to delete its VAO and VBO
      {
          this->model->cleanUp();
      }


    void ForceGenerator::UpdateForce(Particle* p, float time)
    { //placeholder for the class' children
        p->AddForce(MyVector(0, 0, 0));
    }

    GravityForceGenerator::GravityForceGenerator(const MyVector gravity) : Gravity(gravity) {}
    void GravityForceGenerator::UpdateForce(Particle* p, float time)
    {
        if (p->mass <= 0) //if the particle does not have a valid mass, stop
            return;

        MyVector force = Gravity * p->mass;//adds gravity as a force to be taken into account when updating the particle
        p->AddForce(force);
    }

    DragForceGenerator::DragForceGenerator() = default;
    DragForceGenerator::DragForceGenerator(float _k1, float _k2){
        this->k1 = _k1;
        this->k2 = _k2;
    }
    void DragForceGenerator::UpdateForce(Particle * p, float time)
    {   //simulate drag via damping and applies it to the particle
        MyVector force = MyVector(0, 0, 0);
        MyVector currV = p->velocity;

        float mag = currV.Magnitude();
        if (mag <= 0)
            return;

        float dragF = (k1 * mag) + (k2 * mag);
        MyVector dir = currV.Direction();

        p->AddForce(dir * -dragF);
    }

    void ForceRegistry::Add(Particle* particle, ForceGenerator* generator)
    {
        ParticleForceRegistry toAdd;
        toAdd.particle = particle;
        toAdd.generator = generator;
        //registers particles and the force affecting it
        Registry.push_back(toAdd);
    }
    void ForceRegistry::Remove(Particle* particle, ForceGenerator* generator)
    {
        //removes particles as indicated by the ones passed to this function 
        Registry.remove_if([particle, generator](ParticleForceRegistry reg)
            {return reg.particle == particle && reg.generator == generator; });
    }
    void ForceRegistry::Clear()
    { //empties the registry (but doesn't destroy the contents)
        Registry.clear();
    }
    void ForceRegistry::UpdateForces(float time)
    {            //adds the force associated to the accumulatedForce of the particle
        for (std::list<ParticleForceRegistry>::iterator i = Registry.begin(); i != Registry.end(); i++)
            i->generator->UpdateForce(i->particle, time);
        
    }

    PhysicsWorld::PhysicsWorld() = default;
    PhysicsWorld::PhysicsWorld(Model * m, DragForceGenerator * d) : model(m), drag(d) {}
    void PhysicsWorld::AddParticle(Particle * toAdd)
    {   //adds partrticles to the world and applies gravity
        this->Particles.push_back(toAdd);
        forceRegistry.Add(toAdd, &Gravity);
    }
    void PhysicsWorld::Draw(GLuint * shaderProg, Camera * cam)
    {
        //draws particles on the screen
        for (std::list<Particle*>::iterator p = Particles.begin(); p != Particles.end(); p++)
        {
            (*p)->draw(shaderProg, cam);
        }
    }
    void PhysicsWorld::Update(float Time)
    {
        UpdateParticleList(); //updates list and removes unneeded particles

        //updates particles depending on the forces applied
        forceRegistry.UpdateForces(Time);
        for (std::list<Particle*>::iterator p = Particles.begin(); p != Particles.end(); p++)
        {
            (*p)->update(Time);
        }
    }
    void PhysicsWorld::UpdateParticleList()
        {//removes particles depending on whether they are to be destroyed or not and updates the list accordingly


            int SizeBefore = this->Particles.size();//check current number of sparks
            Particles.remove_if([](Particle* p) { return p->destroy(); });
            int SizeAfter = this->Particles.size();//check current number of sparks after removal

            if (SizeBefore != SizeAfter) //if they are different
            {
                srand(time(NULL));
                for (int i = 0; i < (SizeBefore - SizeAfter); i++)//generate new particles to replace the ones that were lost
                {
                    Particle* p = new Particle(model);
                    this->forceRegistry.Add(p, drag); //applies drag
                    this->AddParticle(p);;
                }
            }



        }

