#ifndef __P1_h
#define __P1_h

#include "Primitive.h"
#include "Scene.h"
#include "graphics/Application.h"

using namespace cg;

class P1 : public GLWindow
{
public:
    P1(int width, int height) :
        GLWindow{ "cg2019 - P1", width, height },
        _program{ "P1" }
    {
        // do nothing
    }

    /// Initialize the app.
    void initialize() override;

    /// Update the GUI.
    void gui() override;

    /// Render the scene.
    void render() override;

private:
    GLSL::Program _program;
    Reference<Scene> _scene;
    SceneNode* _current{};
    Color selectedWireframeColor{ 255, 102, 0 };
    mat4f _transform{ mat4f::identity() };

    /// Add a new scene object to current node if it is a Scene or a SceneObject.
    /// It returns the same scene object if the operation succeeded
    /// or nullptr if it could add to current node.
    SceneObject* add_to_current_node(SceneObject*, bool = false);

    void buildScene();

    void hierarchyWindow();
    void inspectorWindow();
    void sceneGui();
    void sceneObjectGui();
    void objectGui();

    void hierarchy_tree_from(SceneNode*);

    SceneObject* new_empty_object() const;
    SceneObject* new_box() const;
	SceneObject* new_sphere() const;
}; // P1

#endif // __P1_h
