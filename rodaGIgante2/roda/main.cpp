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
#include <vart/light.h>
#include <vart/meshobject.h>
#include <vart/contrib/viewerglutogl.h>
#include <vart/contrib/mousecontrol.h>
#include <vart/transform.h>
#include <vart/arrow.h>

#include <iostream>
#include <vector>

using namespace std;

const int numeroCadeiras = 10;
const double anguloEntreCadeiras = ((M_PI * 2)/numeroCadeiras);


class MyIHClass : public VART::ViewerGlutOGL::IdleHandler
{
    public:
        vector<VART::Transform*> tCadeiras;
        VART::Transform* rRoda;
        
        MyIHClass(){}
        virtual ~MyIHClass() {}
        virtual void OnIdle() {
            rRoda->MakeRotation(VART::Point4D::X(), rotacaoRoda);

            double anguloTCadeiras = 0;

            for (int i = 0; i < numeroCadeiras; i++) {
                double y = 70 * sin( anguloTCadeiras + rotacaoRoda );
                double z = 70 * -cos( anguloTCadeiras + rotacaoRoda );

                tCadeiras[i]->MakeTranslation(0, y, z);

                anguloTCadeiras += anguloEntreCadeiras;
            }


            rotacaoRoda += 0.03;
            viewerPtr->PostRedisplay();
        }
        VART::Transform* rotationPtr;
        VART::Transform* translationPtr;
    protected:
    private:
        double rotacaoRoda = 0;
};


int main(int argc, char* argv[])
{
    VART::ViewerGlutOGL::Init(&argc, argv); // Initialize GLUT


    static VART::Scene scene; // create a scene
    static VART::ViewerGlutOGL viewer; // create a viewer (application window)

    VART::Camera camera(VART::Point4D(200,60,0),VART::Point4D::ORIGIN(),VART::Point4D::Y());
    camera.SetFarPlaneDistance(1000);

    // Create some objects
    VART::Arrow arrowX(2);
    VART::Arrow arrowY(VART::Point4D::ORIGIN(), VART::Point4D::Y()*2);
    VART::Arrow arrowZ(VART::Point4D::ORIGIN(), VART::Point4D::Z()*2);

    // Initialize scene objects
    arrowX.SetMaterial(VART::Material::PLASTIC_RED()); //DIREITA/ESQUERDA
    arrowY.SetMaterial(VART::Material::PLASTIC_GREEN()); //CIMA
    arrowZ.SetMaterial(VART::Material::PLASTIC_BLUE()); //APONTANDO PARA MIM

    // Build the scene graph
    scene.AddObject(&arrowX);
    scene.AddObject(&arrowY);
    scene.AddObject(&arrowZ);

    VART::MeshObject* suporte;
    VART::MeshObject* roda;
    VART::MeshObject* cadeira;

    list<VART::MeshObject*> objects;

    // Initialize scene objects
    VART::MeshObject::ReadFromOBJ("ferris-wheel.obj", &objects);

    // Build up the scene
    list<VART::MeshObject*>::iterator iter = objects.begin();
    for (; iter != objects.end(); ++iter) {
        if ((*iter)->GetDescription() == "support") {
        suporte = *iter;
    } else if ((*iter)->GetDescription() == "wheel") {
        roda = *iter;
    } else if ((*iter)->GetDescription() == "chair") {
        cadeira = *iter;
    }
        
    }

    //T -> R -> E (FORMATO GRAFO CENA)
    //E -> R -> T (conceito)

    //scene.AddObject(suporte);


    //Transformações cadeiras
    vector<VART::Transform*> tCadeiras;
    
    for (unsigned i =0; i < numeroCadeiras; i++) {
        VART::Transform* tCadeira = new VART::Transform();
        tCadeira->MakeIdentity();
        tCadeira->AddChild(*cadeira);

        tCadeiras.push_back(tCadeira);
        scene.AddObject(tCadeiras[i]);

    }
   //TRansformações da Roda
    VART::Transform* targetRoda = new VART::Transform();
    VART::Transform* targetSuporte = new VART::Transform();

    targetRoda->MakeIdentity();
    targetSuporte->MakeIdentity();
    targetSuporte->AddChild(*suporte);
    targetRoda->AddChild(*roda);
    scene.AddObject(targetRoda);
    scene.AddObject(targetSuporte);

    MyIHClass idle;
    idle.rRoda = targetRoda;
    idle.tCadeiras= tCadeiras;
    viewer.SetIdleHandler(&idle);


    scene.AddLight(VART::Light::BRIGHT_AMBIENT());
    scene.AddCamera(&camera);
    //scene.MakeCameraViewAll();

    // Set up the viewer
    viewer.SetTitle("Roda Gigante");
    viewer.SetScene(scene); // attach the scene
    scene.DrawLightsOGL(); // Set OpenGL's lights' state
    VART::ViewerGlutOGL::MainLoop(); // Enter main loop (event loop)
    return 0;
}