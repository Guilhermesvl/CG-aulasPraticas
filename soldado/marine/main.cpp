// main.cpp - Test application for JointAction
// This application shows how to make simple animation using the class JointAction.

// Changelog
// Oct 19, 2012 - Bruno de Oliveira Schneider
// - Adapted to new keyboard handler methods.
// Jul 15, 2008 - Bruno Schneider
// - Application created.

#include <vart/scene.h>
#include <vart/arrow.h>
#include <vart/transform.h>
#include <vart/joint.h>
#include <vart/jointmover.h>
#include <vart/uniaxialjoint.h>
#include <vart/baseaction.h>
#include <vart/jointaction.h>
#include <vart/light.h>
#include <vart/sphere.h>
#include <vart/callback.h>
#include <vart/meshobject.h>
#include <vart/box.h>
#include <vart/linearinterpolator.h>
#include <vart/contrib/viewerglutogl.h>


#include <iostream>
#include <vector>

using namespace std;
using VART::Point4D;


const unsigned int MAX_KEYS = 10;
enum Key { UP, DOWN, RIGHT, LEFT };
bool keyPressed[MAX_KEYS];

// Define the keyboard handler
class KeyboardHandler : public VART::ViewerGlutOGL::KbHandler
{
    public:
        KeyboardHandler() {
            for (unsigned int i = 0; i < MAX_KEYS; ++i)
                keyPressed[i] = false;
        }
        virtual void OnKeyDown(int key) {
            switch (key) {
                case VART::KEY_LEFT:
                    keyPressed[LEFT] = true;
                    break;
                case VART::KEY_RIGHT:
                    keyPressed[RIGHT] = true;
                    break;
                case VART::KEY_UP:
                    keyPressed[UP] = true;
                    break;
                case VART::KEY_DOWN:
                    keyPressed[DOWN] = true;
                    break;
            }
        }
        virtual void OnKeyUp(int key) {
            switch (key) {
                case VART::KEY_LEFT:
                    keyPressed[LEFT] = false;
                    break;
                case VART::KEY_RIGHT:
                    keyPressed[RIGHT] = false;
                    break;
                case VART::KEY_UP:
                    keyPressed[UP] = false;
                    break;
                case VART::KEY_DOWN:
                    keyPressed[DOWN] = false;
                    break;
            }
        }
    private:
};

class MyIHClass : public VART::ViewerGlutOGL::IdleHandler
{
    public:
        MyIHClass() : translacaoSoldado(NULL), rotacaoSoldado(NULL), angulo(0){
        }
        virtual ~MyIHClass() {}
        virtual void OnIdle() {
            bool movimentou = false;

            if (keyPressed[UP]) {

                Point4D delta(cos(angulo) * 0.01, 0, -sin(angulo) * 0.01, 0);
                Point4D pos;
                translacaoSoldado->GetTranslation(&pos);
                translacaoSoldado->MakeTranslation(pos + delta);
                movimentou = true;
            }
            if (keyPressed[DOWN]) {
                Point4D delta(-cos(angulo) * 0.01, 0, sin(angulo) * 0.01, 0);
                Point4D pos;
                translacaoSoldado->GetTranslation(&pos);
                translacaoSoldado->MakeTranslation(pos + delta);
                movimentou = true;
            }
            if (keyPressed[LEFT]) {
                angulo += 0.05;
                rotacaoSoldado->MakeYRotation(angulo);
                movimentou = true;
            }
            if (keyPressed[RIGHT]) {
                angulo -= 0.05;
                rotacaoSoldado->MakeYRotation(angulo);
                movimentou = true;
            }

            if (movimentou)
                viewerPtr->PostRedisplay();
        }

        VART::Transform* translacaoSoldado;
        VART::Transform* rotacaoSoldado;
        float angulo;
    protected:
    private:
        Point4D posicaoAtual;
};



// The application itself:
int main(int argc, char* argv[])
{
    VART::ViewerGlutOGL::Init(&argc, argv); // Initialize GLUT
    static VART::Scene scene; // create a scene
    static VART::ViewerGlutOGL viewer; // create a viewer (application window)

    
    VART::Camera camera(Point4D(0,6,16), Point4D::ORIGIN(), Point4D::Y());
    MyIHClass* idleHandler = new MyIHClass();
    KeyboardHandler* kbh = new KeyboardHandler(); 
    
    VART::Box* box = new VART::Box();
    box->MakeBox(-0.9, 4.9, -0.02, 0, -0.9, 4.9);
    box->SetMaterial(VART::Material::DARK_PLASTIC_GRAY());
    scene.AddObject(box);
    
    // Create some objects
    VART::Arrow arrowX(2);
    VART::Arrow arrowY(VART::Point4D::ORIGIN(), VART::Point4D::Y()*2);
    VART::Arrow arrowZ(VART::Point4D::ORIGIN(), VART::Point4D::Z()*2);

    // Initialize scene objects
    arrowX.SetMaterial(VART::Material::PLASTIC_RED());
    arrowY.SetMaterial(VART::Material::PLASTIC_GREEN());
    arrowZ.SetMaterial(VART::Material::PLASTIC_BLUE());
    
    // Build the scene graph
    scene.AddObject(&arrowX);
    scene.AddObject(&arrowY);
    scene.AddObject(&arrowZ);
    
    
    
    // Initialize scene objects
    list<VART::MeshObject*> objects;
    VART::MeshObject::ReadFromOBJ("marine.obj", &objects);
    
    // Build up the scene
    list<VART::MeshObject*>::iterator iter = objects.begin();
    
    VART::MeshObject* soldado;
    VART::MeshObject* arma;
    
    // Build up the scene
    for (; iter != objects.end(); ++iter) {
        if ((*iter)->GetDescription() == "Marine") {
            soldado = *iter;
        } else if ((*iter)->GetDescription() == "Marine_Gun") {
            arma = *iter;
        }
    }
    
    VART::Transform* tSoldadoEscala = new VART::Transform();
    VART::Transform* tSoldadoRotacaoX = new VART::Transform();
    VART::Transform* tSoldadoRotacaoY = new VART::Transform();
    VART::Transform* tSoldadoTranslacao = new VART::Transform();

    tSoldadoTranslacao->MakeIdentity(); 
    tSoldadoEscala->MakeScale(0.755, 0.755, 0.755);
    tSoldadoRotacaoY->MakeYRotation(0); 
    tSoldadoRotacaoX->MakeXRotation(-M_PI_2); 

    //(T -> S -> Ry -> Rx -> Mesh)
    tSoldadoTranslacao->AddChild(*tSoldadoEscala);
    tSoldadoEscala->AddChild(*tSoldadoRotacaoY);
    tSoldadoRotacaoY->AddChild(*tSoldadoRotacaoX);
    tSoldadoRotacaoX->AddChild(*arma);
    tSoldadoRotacaoX->AddChild(*soldado);

    scene.AddObject(tSoldadoTranslacao);

    idleHandler->translacaoSoldado = tSoldadoTranslacao;
    idleHandler->rotacaoSoldado = tSoldadoRotacaoY;

    
    scene.AddCamera(&camera);
    
    // Set up the viewer
    viewer.SetScene(scene); // attach the scene
    //scene.MakeCameraViewAll();
    camera.SetNearPlaneDistance(1);
    
    viewer.SetKbHandler(kbh);
    viewer.SetIdleHandler(idleHandler);
    
    viewer.SetTitle("JointAction Example");
    
    
    // Run application
    scene.DrawLightsOGL(); // Set OpenGL's lights' state
    VART::ViewerGlutOGL::MainLoop(); // Enter main loop (event loop)
    return 0;
}