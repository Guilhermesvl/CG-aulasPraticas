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
#include <cmath> // Para M_PI

using namespace std;

class MyIHClass : public VART::ViewerGlutOGL::IdleHandler
{
    public:
        MyIHClass() : grausEixo(0), grausGrade(0), giraParaFrente(true) {}
        virtual ~MyIHClass() {}
        virtual void OnIdle() {
            grausEixo += 2;
            rotacaoEixo->MakeRotation(VART::Point4D(-0.662, 1.227, 0), VART::Point4D::X(), grausEixo);

            if (giraParaFrente) {
                grausGrade += 0.005;
                if (grausGrade >= M_PI) {
                    grausGrade = M_PI;
                    giraParaFrente = false;
                }
            } else {
                grausGrade -=0.005;
                if (grausGrade <= 0.0) {
                    grausGrade = 0.0;
                    giraParaFrente = true;
                }
            }

            rotacaoGrade->MakeYRotation(grausGrade);
            viewerPtr->PostRedisplay();
        }
    //protected:
        VART::Transform* rotacaoEixo;
        VART::Transform* rotacaoGrade;
    private:
        float grausEixo;
        float grausGrade;
        bool giraParaFrente;
};
// O restante do seu código main.cpp permanece o mesmo...
// The application itself:
int main(int argc, char* argv[])
{
    VART::ViewerGlutOGL::Init(&argc, argv); // Initialize GLUT

    static VART::Scene scene; // create a scene
    static VART::ViewerGlutOGL viewer; // create a viewer (application window)

    // create a camera (scene observer)
    VART::Camera camera(VART::Point4D(0,5,3),VART::Point4D(0,0,0),VART::Point4D(0,1,0,0));

    // Create some objects
    VART::Arrow arrowX(2);
    VART::Arrow arrowY(VART::Point4D::ORIGIN(), VART::Point4D::Y()*2);
    VART::Arrow arrowZ(VART::Point4D::ORIGIN(), VART::Point4D::Z()*2);

    // Initialize scene objects
    arrowX.SetMaterial(VART::Material::PLASTIC_RED());
    arrowY.SetMaterial(VART::Material::PLASTIC_GREEN());
    arrowZ.SetMaterial(VART::Material::PLASTIC_BLUE());

    // Build the scene graph
    //scene.AddObject(&arrowX);
    //scene.AddObject(&arrowY);
    //scene.AddObject(&arrowZ);

    // create some objects
    list<VART::MeshObject*> objects;
    VART::MeshObject* helice;
    VART::MeshObject* base;
    VART::MeshObject* grade;
    VART::MeshObject* suporteEixo;
    VART::MeshObject* eixo;


    // Initialize scene objects
    VART::MeshObject::ReadFromOBJ("fan.obj", &objects);

    // Build up the scene
    list<VART::MeshObject*>::iterator iter = objects.begin();
    for (; iter != objects.end(); ++iter) {
        if ((*iter)->GetDescription() == "helice") {
            helice = *iter;
        } else if ((*iter)->GetDescription() == "base") {
            base = *iter;
        } else if ((*iter)->GetDescription() == "grade") {
            grade = *iter;
        } else if ((*iter)->GetDescription() == "suporte-eixo") {
            suporteEixo = *iter;
        } else if ((*iter)->GetDescription() == "eixo") {
            eixo = *iter;
        }
    }   

    VART::Transform* targetGrade = new VART::Transform();
    VART::Transform* targetEixo = new VART::Transform();

    targetEixo->MakeIdentity();
    targetGrade->MakeIdentity();

    targetGrade->AddChild(*grade);
    targetGrade->AddChild(*suporteEixo);
    targetGrade->AddChild(*targetEixo);
    targetEixo->AddChild(*helice);
    targetEixo->AddChild(*eixo);

    scene.AddObject(base);
    scene.AddObject(targetGrade);

    MyIHClass idle;
    idle.rotacaoEixo = targetEixo;
    idle.rotacaoGrade = targetGrade;

    scene.AddLight(VART::Light::BRIGHT_AMBIENT());
    scene.AddCamera(&camera);
    //scene.MakeCameraViewAll();

    // Set up the viewer
    viewer.SetTitle("ventilador");
    viewer.SetScene(scene); // attach the scene
    viewer.SetIdleHandler(&idle);

    // Run application
    scene.DrawLightsOGL(); // Set OpenGL's lights' state
    VART::ViewerGlutOGL::MainLoop(); // Enter main loop (event loop)
    return 0;
}