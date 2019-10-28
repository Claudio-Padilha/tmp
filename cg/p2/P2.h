#ifndef __P2_h
#define __P2_h

#include "Assets.h"
#include "GLRenderer.h"
#include "Primitive.h"
#include "SceneEditor.h"
#include "core/Flags.h"
#include "graphics/Application.h"
#include <vector>

using namespace cg;

class P2: public GLWindow
{
public:
  P2(int width, int height):
    GLWindow{"cg2019 - P2", width, height},
    _program{"P2"}
  {
    // do nothing
  }

  /// Initialize the app.
  void initialize() override;

  /// Update the GUI.
  void gui() override;

  /// Render the scene.
  void render() override;

  void terminate() override;

private:
  enum ViewMode
  {
    Editor = 0,
    Renderer = 1
  };

  enum class MoveBits
  {
    Left = 1,
    Right = 2,
    Forward = 4,
    Back = 8,
    Up = 16,
    Down = 32
  };

  enum class DragBits
  {
    Rotate = 1,
    Pan = 2
  };

  GLSL::Program _program;
  Reference<Scene> _scene;
  Reference<SceneEditor> _editor;
  Reference<GLRenderer> _renderer;
  SceneNode* _current{};
  Color _selectedWireframeColor{255, 102, 0};
  Flags<MoveBits> _moveFlags{};
  Flags<DragBits> _dragFlags{};
  int _pivotX;
  int _pivotY;
  int _mouseX;
  int _mouseY;
  bool _showAssets{true};
  bool _showEditorView{true};
  ViewMode _viewMode{ViewMode::Editor};

  // Perhaps it should be removed soon
  GLuint _fbo = 0;
  GLuint _tex[2] = { 0 };

  static MeshMap _defaultMeshes;

  void buildScene();
  void renderScene();

  void mainMenu();
  void fileMenu();
  void showOptions();

  void hierarchyWindow();
  void inspectorWindow();
  void assetsWindow();
  void editorView();
  void sceneGui();
  void sceneObjectGui();
  void objectGui();
  void editorViewGui();
  void inspectPrimitive(Primitive&);
  void inspectCamera(Camera&);
  void addComponentButton(SceneObject&);

  void drawPrimitive(Primitive&, bool = false);
  void drawCamera(Camera&);

  bool windowResizeEvent(int, int) override;
  bool keyInputEvent(int, int, int) override;
  bool scrollEvent(double, double) override;
  bool mouseButtonInputEvent(int, int, int) override;
  bool mouseMoveEvent(double, double) override;

  static void buildDefaultMeshes();

  /// Add a new scene object to current node if it is a Scene or a SceneObject.
  /// It returns the same scene object if the operation succeeded
  /// or nullptr if it could add to current node.
  SceneObject* add_to_current_node(SceneObject*, bool = false);

  void hierarchy_tree_from(SceneNode*);
  void hierarchy_tree_drag_drop(Reference<SceneNode>);

  Reference<SceneObject> new_empty_object() const;
  Reference<SceneObject> new_box() const;
  Reference<SceneObject> new_sphere() const;
  Reference<SceneObject> new_camera() const;

  void no_current_camera_fallback();
  void preview(Camera&);
  void render_scene_object(const SceneObject*, bool = false, bool = true);
}; // P2

#endif // __P2_h
