// main.cpp - Test application for JointAction

// This application shows how to make simple animation using the class JointAction.

// Changelog
// Oct 19, 2012 - Bruno de Oliveira Schneider
// - Adapted to new keyboard handler methods.
// Jul 15, 2008 - Bruno Schneider
// - Application created.

#include <vart/arrow.h>
#include <vart/scene.h>
#include <vart/light.h>
#include <vart/mesh.h>
#include <vart/meshobject.h>
#include <vart/contrib/viewerglutogl.h>

#include <vector>
#include <iostream>
#include <cmath>

using namespace std;

using VART::Point4D;

// The application itself:
int main(int argc, char* argv[])
{
    VART::ViewerGlutOGL::Init(&argc, argv); // Initialize GLUT
    VART::Camera camera(VART::Point4D(15, 25, 25),VART::Point4D(7.5, 0, 7.5),VART::Point4D::Y());
    
    static VART::Scene scene; // create a scene
    static VART::ViewerGlutOGL viewer; // create a viewer (application window)

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
    
    
    
    int numLinhas = 20;
    int numColunas = 20;
    
    vector<vector<VART::Point4D>> pontos(numLinhas, vector<VART::Point4D>(numColunas));
    vector<VART::Point4D> vertices;
    vertices.reserve(numLinhas * numColunas);

    srand(time(0));
    
    for (int i = 0; i < numLinhas; ++i) {
        for (int j = 0; j < numColunas; ++j) {
            float x = j;
            float z = i;
            float y = 0;

            float yCima = (i > 0) ? pontos[i-1][j].GetY() : 0;
            float yEsquerda = (j > 0) ? pontos[i][j-1].GetY() : 0;

            float mediaAltura = (i > 0 && j > 0) ? (yCima + yEsquerda) / 2.0f : (i > 0 ? yCima : yEsquerda);
            
            // Garante inclinação máxima de 45° (deltaY <= 1)
            int delta = rand() % 3 - 1; // -1, 0 ou 1
            y = mediaAltura + delta;

            Point4D ponto(x, y, z);
            pontos[i][j] = ponto;
            vertices.push_back(ponto);
        }
    }


    VART::MeshObject terreno;
    terreno.SetVertices(vertices);

    int ultimaLinha = numLinhas - 1;
    int linha = 0;

    while(linha < ultimaLinha) {
        VART::Mesh fitaTriangulo;
        fitaTriangulo.type = VART::Mesh::TRIANGLE_STRIP;
    
        for (int coluna = 0; coluna < numColunas; coluna++) {
            fitaTriangulo.indexVec.push_back(linha * numColunas + coluna);
            fitaTriangulo.indexVec.push_back((linha+1) * numColunas + coluna);
        }

        terreno.AddMesh(fitaTriangulo);
        linha++;

    }

    terreno.ComputeVertexNormals();
    terreno.SetMaterial(VART::Material::PLASTIC_GREEN());
    scene.AddObject(&terreno);

    scene.AddLight(VART::Light::BRIGHT_AMBIENT());
    scene.AddCamera(&camera);

    // Set up the viewer
    viewer.SetScene(scene); // attach the scene
    viewer.SetTitle("Terreno");


    // Run application
    scene.DrawLightsOGL(); // Set OpenGL's lights' state
    VART::ViewerGlutOGL::MainLoop(); // Enter main loop (event loop)
    return 0;
}