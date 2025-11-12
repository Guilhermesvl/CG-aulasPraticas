// main.cpp - Wavefront OBJ viewer application
// V-ART example: Load and Display a OBJ file.
// This application shows how to create an object from a Wavefront OBJ file.
// Made from picker example, keeps the piking code.

// Changelog
// Oct 19, 2012 - Bruno de Oliveira Schneider
// - Adapted to new keyboard handler methods.
// Sep 26, 2012 - Bruno de Oliveira Schneider
// - Application created.

#include <vart/scene.h>
#include <vart/box.h>
#include <vart/cylinder.h>
#include <vart/light.h>
#include <vart/meshobject.h>
#include <vart/sphere.h>
#include <vart/contrib/viewerglutogl.h>
#include <vart/contrib/mousecontrol.h>
#include <vart/arrow.h>

#include <iostream>

using namespace std;



// The application itself:
int main(int argc, char* argv[])
{
    VART::ViewerGlutOGL::Init(&argc, argv); // Initialize GLUT

    static VART::Scene scene; // create a scene
    static VART::ViewerGlutOGL viewer; // create a viewer (application window)

    // create a camera (scene observer)
    VART::Camera camera(VART::Point4D(0, 10, 10),VART::Point4D(0, 0, -15),VART::Point4D(0, 1, 0, 0));

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

    // create some objects
    list<VART::MeshObject*> objects;
    VART::MeshObject* rua = nullptr;
    VART::MeshObject* poste = nullptr; 


    // Initialize scene objects
    VART::MeshObject::ReadFromOBJ("street.obj", &objects);
    VART::MeshObject::ReadFromOBJ("light-pole.obj", &objects);

    // Build up the scene
    list<VART::MeshObject*>::iterator iter = objects.begin();
    for (; iter != objects.end(); ++iter) {
       if ((*iter)->GetDescription() == "light-pole") {
            poste = *iter;
        } else if ((*iter)->GetDescription() == "Street") {
            rua = *iter;
        }
    }

    
    int qtRua = 3;
    int distanciaRua = -16;
    
    for (int i = 0; i < qtRua; i++) {
        VART::Transform* transladaRua = new VART::Transform();
        transladaRua->MakeTranslation(0, 0, i * distanciaRua);
        transladaRua->AddChild(*rua);
        
        scene.AddObject(transladaRua);
    }

    double distanciaZ = -30;
    int distanciaX = 4.0;

    // --- CORREÇÃO LOOP POSTES ---
    // A rotação em X (rotacionaPosteX) foi restaurada para "levantar" o poste.
    // A rotação em Y (rotacionaPosteY) foi corrigida para virar o braço para a rua.
    for (int i = 0; i < 4; i++) {
        VART::Transform* transladaPoste = new VART::Transform();
        VART::Transform* rotacionaPosteX = new VART::Transform();
        VART::Transform* rotacionaPosteY = new VART::Transform();


        double posicaoX = (i % 2 == 0 ? -1 : 1) * distanciaX;
        double posicaoZ = (i / 2) * distanciaZ;

        // Define as transformações
        transladaPoste->MakeTranslation(posicaoX, 0, posicaoZ);
        rotacionaPosteX->MakeXRotation(M_PI_2); // 1. Levanta o poste (gira em torno de X)

        // 2. Gira o poste (em torno de Y) para o braço apontar para a rua
        // (Assumindo que o braço, após levantar, aponta para -Z)
        if (i % 2 == 0) { 
            // Postes da esquerda (i=0, 2)
            // Gira +90 graus para o braço apontar para +X (rua)
            rotacionaPosteY->MakeYRotation(0); 
        } else {
            // Postes da direita (i=1, 3)
            // Gira -90 graus para o braço apontar para -X (rua)
            rotacionaPosteY->MakeYRotation(M_PI);
        }

        transladaPoste->AddChild(*rotacionaPosteY);
        rotacionaPosteY->AddChild(*rotacionaPosteX);
        rotacionaPosteX->AddChild(*poste);

        scene.AddObject(transladaPoste);
    }


    scene.AddLight(VART::Light::BRIGHT_AMBIENT());
    scene.AddCamera(&camera);
   // scene.MakeCameraViewAll();

    // Set up the viewer
    viewer.SetTitle("Rua");
    viewer.SetScene(scene); // attach the scene

    // Run application
    scene.DrawLightsOGL(); // Set OpenGL's lights' state
    VART::ViewerGlutOGL::MainLoop(); // Enter main loop (event loop)
    return 0;
}