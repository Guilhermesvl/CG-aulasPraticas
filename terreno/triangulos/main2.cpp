// main.cpp - from V-ART template application

// This example shows how to use the Arrow class.

// Changelog
// Oct 19, 2012 - Bruno de Oliveira Schneider
// - Removed the unused keyboard handler.
// - Added more arrows.
// Sep 25, 2008 - Bruno de Oliveira Schneider
// - Application created.

#include <vart/scene.h>
#include <vart/light.h>
#include <vart/arrow.h>
#include <vart/meshobject.h>
#include <vart/sphere.h>
#include <vart/contrib/viewerglutogl.h>
#include <vart/contrib/mousecontrol.h>

#include <vector>
#include <iostream>
#include <cstdlib>  // rand
#include <ctime>  

using namespace std;

// Define the click handler
class ClickHandlerClass : public VART::MouseControl::ClickHandler
{
    public:
        ClickHandlerClass() {
        }
        virtual ~ClickHandlerClass() {};
        virtual void OnClick() {
            if (mouseCtrlPtr->LastClickIsDown()) {
            }
        }
};

// The application itself:
int main(int argc, char* argv[])
{
    VART::ViewerGlutOGL::Init(&argc, argv); // Initialize GLUT
    static VART::Scene scene; // create a scene
    static VART::ViewerGlutOGL viewer; // create a viewer (application window)
    // create a camera (scene observer)
    VART::Camera camera(VART::Point4D(0,0,6),VART::Point4D::ORIGIN(),VART::Point4D::Y());
    camera.SetFarPlaneDistance(0);

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

    const int numeroLinhas = 20;
    const int numeroColunas = 20;

    vector<vector<VART::Point4D>> pontos (numeroLinhas, vector<VART::Point4D>(numeroColunas));
    vector<VART::Point4D> vertices;
    vertices.reserve(numeroLinhas * numeroColunas);


    srand(time(0)); // Para alturas aleatórias

    // Geração dos pontos
    for (int i = 0; i < numeroLinhas; ++i) {
        for (int j = 0; j < numeroColunas; ++j) {
            float x = j;
            float z = i;
            float y = rand() % 5; // Altura aleatória de 0 a 4
            vertices.emplace_back(x, y, z);
        }
    }


    VART::MeshObject terreno;
    terreno.indexVec.SetVertices(vertices);

    int ultimaLinha = numeroLinhas -1;
    int linha = 0;
    int coluna = 0;

    while (linha < ultimaLinha) {
        VART::Mesh fitaTriangulos;
        fitaTriangulos.type = VART::Mesh::TRIANGLE_STRIP;

        while (coluna < numeroColunas) {
            fitaTriangulos.indexVec.push_back(linha * numeroColunas + coluna);
            fitaTriangulos.indexVec.push_back((linha+1)* numeroColunas + coluna);
            coluna++;
        }
        terreno.AddMesh(fitaTriangulos);
        linha++;
    }

    
    terreno.ComputeVertexNormals();
    terreno.SetMaterial(VART::Material::PLASTIC_GREEN());
    scene.AddObject(&terreno);

    // Add lights and cameras to the scene
    scene.AddLight(VART::Light::BRIGHT_AMBIENT());
    scene.AddCamera(&camera);

    // Set up the viewer
    viewer.SetTitle("V-ART arrow example");
    viewer.SetScene(scene); // attach the scene

    // Run application
    scene.DrawLightsOGL(); // Set OpenGL's lights' state
    VART::ViewerGlutOGL::MainLoop(); // Enter main loop (event loop) and never return
    return 0;
}
