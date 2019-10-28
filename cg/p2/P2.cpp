#include "geometry/MeshSweeper.h"
#include "P2.h"

#include <algorithm>
#include <limits>
#include <utility>
#include <string.h>

MeshMap P2::_defaultMeshes;

SceneObject*
P2::add_to_current_node(SceneObject* obj, bool replace_current)
{
    auto current_scene_object = dynamic_cast<SceneObject*>(_current);
    obj->setParent(current_scene_object);

    // Not a scene object?
    if (!current_scene_object)
    {
        auto current_scene = dynamic_cast<Scene*>(_current);
        // Neither a scene object nor a scene?
        if (!current_scene)
            return nullptr; // Could not add it: failed
    }

    if (replace_current)
        _current = obj;

    return obj;
}

inline void
P2::buildDefaultMeshes()
{
    _defaultMeshes["None"] = nullptr;
    _defaultMeshes["Box"] = GLGraphics3::box();
    _defaultMeshes["Sphere"] = GLGraphics3::sphere();
}

inline Primitive*
makePrimitive(MeshMapIterator mit)
{
    return new Primitive{ mit->second, mit->first };
}

inline void
P2::buildScene()
{
    _current = _scene = new Scene{ "Scene 1" };
    _editor = new SceneEditor{ *_scene };
    _editor->setDefaultView((float)width() / (float)height());

    // Use `auto` no lugar de `Reference` e veja a mágica acontecer!
    //      Este é o perigo de utilizar uma SharedObject com o contador em zero!
    //      É instável!
    // auto o = new SceneObject{ "Main Camera", *_scene };
    Reference o = new SceneObject{ "Main Camera", *_scene };
    Reference camera = new Camera;

    o->add_component(camera);
    o->setParent(nullptr);
    Camera::setCurrent(camera);

    // Sets up new objects with some translation
    auto setup = [&](auto collection, bool replace)
    {
        for (auto [obj, delta] : collection)
        {
            add_to_current_node(obj, replace);
            obj->transform()->setLocalPosition(delta);
        }
    };

    using std::pair;
    auto level01 =
    {
        pair{new_sphere(),      vec3f( 4,0,0)},
        pair{new_box(),         vec3f(-4,0,0)},
        pair{new_empty_object(),vec3f( 0,0,4)},
    };
    auto level02 =
    {
        pair{ new_box(), vec3f(0,2,1) },
        pair{ new_box(), vec3f(0,1,1) },
    };
    auto level03 =
    {
        pair{new_box(),     vec3f( 1,1,0)},
        pair{new_sphere(),  vec3f(-1,0,0)},
        pair{new_sphere(),  vec3f( 0,.5,-1)},
    };

    setup(level01, false);

    auto t = add_to_current_node(new_empty_object(), true)->transform();

    setup(level02, false);
    setup(level03, true);

    t->setLocalPosition(vec3f(-1, 0, -1.5));
    t->setLocalEulerAngles(vec3f(0,45,-70));

    _current = _scene;
}

void
P2::initialize()
{
    Application::loadShaders(_program, "shaders/p2.vs", "shaders/p2.fs");
    Assets::initialize();
    buildDefaultMeshes();
    buildScene();
    _renderer = new GLRenderer{ *_scene };
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    _program.use();

    // P2::A3: maybe it should be removed
    glGenFramebuffers(1, &_fbo);
    glGenTextures(2, _tex);
}

void
P2::terminate()
{
    glDeleteFramebuffers(1, &_fbo);
    glDeleteTextures(2, _tex);
}

namespace ImGui
{
    void ShowDemoWindow(bool*);
}

inline Reference<SceneObject>
P2::new_empty_object() const
{
    static unsigned short id = 0;
    return new SceneObject{
        ("Object " + std::to_string(++id)).c_str(),
        *this->_scene };
}

inline Reference<SceneObject>
P2::new_camera() const
{
    static unsigned short id = 0;
    Reference cam = new SceneObject{
        ("Camera " + std::to_string(++id)).c_str(),
        *this->_scene };

    cam->add_component(new Camera());

    return cam;
}

Reference<SceneObject>
P2::new_box() const
{
    static unsigned short id = 0;
    Reference box = new SceneObject{
        ("Box " + std::to_string(++id)).c_str(),
        *this->_scene };
    box->add_component(makePrimitive(_defaultMeshes.find("Box")));
    return box;
}

Reference<SceneObject>
P2::new_sphere() const
{
    static unsigned short id = 0;
    Reference sphere = new SceneObject{
        ("Sphere " + std::to_string(++id)).c_str(),
        *this->_scene };
    sphere->add_component(makePrimitive(_defaultMeshes.find("Sphere")));
    return sphere;
}

inline void
P2::hierarchyWindow()
{
    ImGui::Begin("Hierarchy");
    if (ImGui::Button("Create###object"))
        ImGui::OpenPopup("CreateObjectPopup");
    if (ImGui::BeginPopup("CreateObjectPopup"))
    {
        if (ImGui::MenuItem("Empty Object"))
        {
            add_to_current_node(new_empty_object(), true);
        }
        if (ImGui::BeginMenu("3D Object"))
        {
            if (ImGui::MenuItem("Box"))
            {
                add_to_current_node(new_box(), true);
            }
            if (ImGui::MenuItem("Sphere"))
            {
                add_to_current_node(new_sphere(), true);
            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Camera"))
        {
            add_to_current_node(new_camera(), true);
        }
        ImGui::EndPopup();
    }
	ImGui::SameLine();
	if (ImGui::Button("Delete"))
	{
        if (auto obj = dynamic_cast<SceneObject*>(_current))
        {
            if (auto parent = obj->parent())
            {
                parent->remove_object(obj);
                _current = parent;
            }
            else if (auto scene = obj->scene())
            {
                scene->remove_object(obj);
                _current = scene;
            }
        }
	}

    ImGui::Separator();

    // DFS to find all scene nodes and fill the imgui tree view with them.
    hierarchy_tree_from(_scene);

    ImGui::End();
}

void P2::hierarchy_tree_from(SceneNode* node)
{
    if (!node) return;

    ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_OpenOnArrow };

    // Get "children" objects if there are any
    const std::vector<Reference<SceneObject>>* objects = nullptr;

    // XXX Still wonder how this code would look like if the scene object collection
    // was declared in the base class.

    // Is it a scene?
    auto scene = dynamic_cast<Scene*>(node);
    if (scene)
    {
        objects = &scene->get_objects();
        flag |= ImGuiTreeNodeFlags_DefaultOpen;
    }

    // Or is it a scene object?
    auto scene_obj = dynamic_cast<SceneObject*>(node);
    if (scene_obj)
        objects = &scene_obj->get_objects();

    auto sub_items = (objects && objects->size() > 0);

    if (!sub_items)
        flag |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    auto open = ImGui::TreeNodeEx(node,
        _current == node ? flag | ImGuiTreeNodeFlags_Selected : flag,
        node->name());

    // Drag and Drop funcionality for parent setting
    hierarchy_tree_drag_drop(node);

    if (ImGui::IsItemClicked())
        _current = node;

    // To next level
    if (sub_items && open)
    {
        for (auto& obj : *objects)
            hierarchy_tree_from(obj);

        ImGui::TreePop();
    }
}

inline void
P2::hierarchy_tree_drag_drop(Reference<SceneNode> node)
{
    constexpr auto PAYLOAD_ID = "SET_PARENT";

    auto obj = dynamic_cast<SceneObject*>(node.get());
    //auto scene = dynamic_cast<SceneObject*>(node.get());

    // Only Scene Objects can be dragged
    if (obj && ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        ImGui::SetDragDropPayload(PAYLOAD_ID, &node, sizeof(node));
        // Tooltop Text
        ImGui::Text("Carrying %s", node->name());
        ImGui::EndDragDropSource();
    }
    if (ImGui::BeginDragDropTarget())
    {
        if (const auto payload = ImGui::AcceptDragDropPayload(PAYLOAD_ID))
        {
            using Ref = Reference<SceneNode>;
            assert(payload->DataSize == sizeof(Ref*));
            
            auto child_ref = *static_cast<Ref*>(payload->Data);
            auto child = dynamic_cast<SceneObject*>(child_ref.get());

            assert(child);

            // Dragged under a Scene Object?
            if (obj)
                child->setParent(obj);
            // Dragged under a Scene?
            else /* if (scene) */
            {
                // XXX And if we are moving the SceneObject between different Scenes?
                child->setParent(nullptr);
            }
        }
        ImGui::EndDragDropTarget();
    }
}

namespace ImGui
{ // begin namespace ImGui

    void
        ObjectNameInput(NameableObject* object)
    {
        const int bufferSize{ 128 };
        static NameableObject* current;
        static char buffer[bufferSize];

        if (object != current)
        {
            strcpy_s(buffer, bufferSize, object->name());
            current = object;
        }
        if (ImGui::InputText("Name", buffer, bufferSize))
            object->setName(buffer);
    }

    inline bool
        ColorEdit3(const char* label, Color& color)
    {
        return ImGui::ColorEdit3(label, (float*)& color);
    }

    inline bool
        DragVec3(const char* label, vec3f& v)
    {
        return DragFloat3(label, (float*)& v, 0.1f, 0.0f, 0.0f, "%.2g");
    }

    void
        TransformEdit(Transform* transform)
    {
        constexpr auto eps = 1e-4f;
        constexpr auto inf = std::numeric_limits<float>::max();

        vec3f temp;

        temp = transform->localPosition();
        if (ImGui::DragVec3("Position", temp))
            transform->setLocalPosition(temp);
        temp = transform->localEulerAngles();
        if (ImGui::DragVec3("Rotation", temp))
            transform->setLocalEulerAngles(temp);
        temp = transform->localScale();
        if (ImGui::DragFloat3("Scale", (float*)&temp,
            0.1f, eps, inf, "%.2g"))
            transform->setLocalScale(temp);
    }

} // end namespace ImGui

inline void
P2::sceneGui()
{
    auto scene = (Scene*)_current;

    ImGui::ObjectNameInput(_current);
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Colors"))
    {
        ImGui::ColorEdit3("Background", scene->backgroundColor);
        ImGui::ColorEdit3("Ambient Light", scene->ambientLight);
    }
}

inline void
P2::inspectPrimitive(Primitive& primitive)
{
    char buffer[16];

    snprintf(buffer, 16, "%s", primitive.meshName());
    ImGui::InputText("Mesh", buffer, 16, ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget())
    {
        if (auto * payload = ImGui::AcceptDragDropPayload("PrimitiveMesh"))
        {
            auto mit = *(MeshMapIterator*)payload->Data;
            primitive.setMesh(mit->second, mit->first);
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::SameLine();
    if (ImGui::Button("...###PrimitiveMesh"))
        ImGui::OpenPopup("PrimitiveMeshPopup");
    if (ImGui::BeginPopup("PrimitiveMeshPopup"))
    {
        auto& meshes = Assets::meshes();

        if (!meshes.empty())
        {
            for (auto mit = meshes.begin(); mit != meshes.end(); ++mit)
                if (ImGui::Selectable(mit->first.c_str()))
                    primitive.setMesh(Assets::loadMesh(mit), mit->first);
            ImGui::Separator();
        }
        for (auto mit = _defaultMeshes.begin(); mit != _defaultMeshes.end(); ++mit)
            if (ImGui::Selectable(mit->first.c_str()))
                primitive.setMesh(mit->second, mit->first);
        ImGui::EndPopup();
    }
    ImGui::ColorEdit3("Mesh Color", (float*)& primitive.color);
}

void
P2::inspectCamera(Camera& camera)
{
    static const char* projectionNames[]{ "Perspective", "Orthographic" };
    auto cp = camera.projectionType();

    if (ImGui::BeginCombo("Projection", projectionNames[cp]))
    {
        for (auto i = 0; i < IM_ARRAYSIZE(projectionNames); ++i)
        {
            auto selected = cp == i;

            if (ImGui::Selectable(projectionNames[i], selected))
                cp = (Camera::ProjectionType)i;
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    camera.setProjectionType(cp);
    if (cp == View3::Perspective)
    {
        auto fov = camera.viewAngle();

        if (ImGui::SliderFloat("View Angle",
            &fov,
            MIN_ANGLE,
            MAX_ANGLE,
            "%.0f deg",
            1.0f))
            camera.setViewAngle(fov <= MIN_ANGLE ? MIN_ANGLE : fov);
    }
    else
    {
        auto h = camera.height();

        if (ImGui::DragFloat("Height",
            &h,
            MIN_HEIGHT * 10.0f,
            MIN_HEIGHT,
            math::Limits<float>::inf()))
            camera.setHeight(h <= 0 ? MIN_HEIGHT : h);
    }

    float n;
    float f;

    camera.clippingPlanes(n, f);

    if (ImGui::DragFloatRange2("Clipping Planes",
        &n,
        &f,
        MIN_DEPTH,
        MIN_DEPTH,
        math::Limits<float>::inf(),
        "Near: %.2f",
        "Far: %.2f"))
    {
        if (n <= 0)
            n = MIN_DEPTH;
        if (f - n < MIN_DEPTH)
            f = n + MIN_DEPTH;
        camera.setClippingPlanes(n, f);
    }
}

inline void
P2::addComponentButton(SceneObject& object)
{
    if (ImGui::Button("Add Component"))
        ImGui::OpenPopup("AddComponentPopup");
    if (ImGui::BeginPopup("AddComponentPopup"))
    {
        if (ImGui::MenuItem("Primitive"))
        {	
			std::string name = object.name();
			Primitive* p;
			
			if (name.find("Box") != std::string::npos)
			{
				p = new Primitive(_defaultMeshes["Box"], object.name());
			}
			else if (name.find("Sphere") != std::string::npos)
			{
				p = new Primitive(_defaultMeshes["Sphere"], object.name());
			}
			else
			{
				p = new Primitive(_defaultMeshes["None"], object.name());
			}
			object.add_component(p);
        }
        if (ImGui::MenuItem("Camera"))
        {
			Camera* cam = new Camera();
			object.add_component(cam);
        }
        ImGui::EndPopup();
    }
}


inline void
P2::sceneObjectGui()
{
    auto object = (SceneObject*)_current;

    addComponentButton(*object);
    ImGui::Separator();
    ImGui::ObjectNameInput(object);
    ImGui::SameLine();
    ImGui::Checkbox("###visible", &object->visible);
    ImGui::Separator();

    for (auto component : object->get_components())
    {
        if (auto t = dynamic_cast<Transform*>(component.get()))
        {
            if (ImGui::CollapsingHeader(t->typeName()))
                ImGui::TransformEdit(t);
        }
        else if (auto p = dynamic_cast<Primitive*>(component.get()))
        {
            auto notDelete{ true };
            auto open = ImGui::CollapsingHeader(p->typeName(), &notDelete);

            if (!notDelete)
            {
				object->remove_component(p);
            }
            else if (open)
                inspectPrimitive(*p);
        }
        else if (auto c = dynamic_cast<Camera*>(component.get()))
        {
            auto notDelete{ true };
            auto open = ImGui::CollapsingHeader(c->typeName(), &notDelete);

            if (!notDelete)
            {
				object->remove_component(c);
            }
            else if (open)
            {
                auto isCurrent = c == Camera::current();

                ImGui::Checkbox("Current", &isCurrent);
                Camera::setCurrent(isCurrent ? c : nullptr);
                inspectCamera(*c);
            }
        }
    }
}

inline void
P2::objectGui()
{
    if (_current == nullptr)
        return;
    if (dynamic_cast<SceneObject*>(_current))
    {
        sceneObjectGui();
        return;
    }
    if (dynamic_cast<Scene*>(_current))
        sceneGui();
}

inline void
P2::inspectorWindow()
{
    ImGui::Begin("Inspector");
    objectGui();
    ImGui::End();
}

inline void
P2::editorViewGui()
{
    if (ImGui::Button("Set Default View"))
        _editor->setDefaultView(float(width()) / float(height()));
    ImGui::Separator();

    auto t = _editor->camera()->transform();
    vec3f temp;

    temp = t->localPosition();
    if (ImGui::DragVec3("Position", temp))
        t->setLocalPosition(temp);
    temp = t->localEulerAngles();
    if (ImGui::DragVec3("Rotation", temp))
        t->setLocalEulerAngles(temp);
    inspectCamera(*_editor->camera());
    ImGui::Separator();
    ImGui::Checkbox("Show Ground", &_editor->showGround);
}

inline void
P2::assetsWindow()
{
    if (!_showAssets)
        return;
    ImGui::Begin("Assets");
    if (ImGui::CollapsingHeader("Meshes"))
    {
        auto& meshes = Assets::meshes();

        for (auto mit = meshes.begin(); mit != meshes.end(); ++mit)
        {
            auto meshName = mit->first.c_str();
            auto selected = false;

            ImGui::Selectable(meshName, &selected);
            if (ImGui::BeginDragDropSource())
            {
                Assets::loadMesh(mit);
                ImGui::Text(meshName);
                ImGui::SetDragDropPayload("PrimitiveMesh", &mit, sizeof(mit));
                ImGui::EndDragDropSource();
            }
        }
    }
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Textures"))
    {
        // p3
    }
    ImGui::End();
}

inline void
P2::editorView()
{
    if (!_showEditorView)
        return;
    ImGui::Begin("Editor View Settings");
    editorViewGui();
    ImGui::End();
}

inline void
P2::fileMenu()
{
    if (ImGui::MenuItem("New"))
    {
        // TODO
    }
    if (ImGui::MenuItem("Open...", "Ctrl+O"))
    {
        // TODO
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Save", "Ctrl+S"))
    {
        // TODO
    }
    if (ImGui::MenuItem("Save As..."))
    {
        // TODO
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Exit", "Alt+F4"))
    {
        shutdown();
    }
}

inline bool
showStyleSelector(const char* label)
{
    static int style = 1;

    if (!ImGui::Combo(label, &style, "Classic\0Dark\0Light\0"))
        return false;
    switch (style)
    {
    case 0: ImGui::StyleColorsClassic();
        break;
    case 1: ImGui::StyleColorsDark();
        break;
    case 2: ImGui::StyleColorsLight();
        break;
    }
    return true;
}

inline void
P2::showOptions()
{
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
    showStyleSelector("Color Theme##Selector");
    ImGui::ColorEdit3("Selected Wireframe", _selectedWireframeColor);
    ImGui::PopItemWidth();
}

inline void
P2::mainMenu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            fileMenu();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            if (Camera::current() == 0)
                ImGui::MenuItem("Edit View", nullptr, true, false);
            else
            {
                static const char* viewLabels[]{ "Editor", "Renderer" };

                if (ImGui::BeginCombo("View", viewLabels[_viewMode]))
                {
                    for (auto i = 0; i < IM_ARRAYSIZE(viewLabels); ++i)
                    {
                        if (ImGui::Selectable(viewLabels[i], _viewMode == i))
                            _viewMode = (ViewMode)i;
                    }
                    ImGui::EndCombo();
                }
            }
            ImGui::Separator();
            ImGui::MenuItem("Assets Window", nullptr, &_showAssets);
            ImGui::MenuItem("Editor View Settings", nullptr, &_showEditorView);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::BeginMenu("Options"))
            {
                showOptions();
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void
P2::gui()
{
    mainMenu();
    hierarchyWindow();
    inspectorWindow();
    assetsWindow();
    editorView();

    /*
    static bool demo = true;
    ImGui::ShowDemoWindow(&demo);
    */
}

inline void
drawMesh(GLMesh* mesh, GLuint mode)
{
    glPolygonMode(GL_FRONT_AND_BACK, mode);
    glDrawElements(GL_TRIANGLES, mesh->vertexCount(), GL_UNSIGNED_INT, 0);
}

inline void
P2::drawPrimitive(Primitive& primitive, bool wireframe)
{
    auto m = glMesh(primitive.mesh());

    if (nullptr == m)
        return;

    auto t = primitive.transform();
    auto normalMatrix = mat3f{ t->worldToLocalMatrix() }.transposed();

    _program.setUniformMat4("transform", t->localToWorldMatrix());
    _program.setUniformMat3("normalMatrix", normalMatrix);
    _program.setUniformVec4("color", primitive.color);
    _program.setUniform("flatMode", (int)0);
    m->bind();
    drawMesh(m, GL_FILL);

    if (wireframe)
    {
        _program.setUniformVec4("color", _selectedWireframeColor);
        _program.setUniform("flatMode", (int)1);
        drawMesh(m, GL_LINE);
    }
}

inline void
P2::drawCamera(Camera& cam)
{
    // Scale to make the far face of the frustum visible
    // XXX Find a better value
    constexpr auto scale = .6f;

    auto height_on = [&](float z) -> float
    {
        return 2.f * z * tan(math::toRadians(cam.viewAngle() / 2.f));
    };

    auto distance = (
        _editor->camera()->transform()->position()
        - cam.transform()->position()
    ).length();

    auto z_near = cam.z_near();
    auto z_far = std::min(
        cam.z_far(),
        std::max(0.f,
            scale * _editor->camera()->z_far() - distance));

    float h_near = 0;
    float h_far = 0;

    switch (cam.projectionType())
    {
    case Camera::Parallel:
        h_far = h_near = cam.height();
        break;
    case Camera::Perspective:
        h_near = height_on(z_near);
        h_far = height_on(z_far);
        break;
    }

    auto j = .5f;
    auto i = j * cam.aspectRatio();

    auto t =  j;    // top
    auto b = -j;    // bottom
    auto l = -i;    // left
    auto r =  i;    // right

	auto to_world = cam.cameraToWorldMatrix();
	
    vec3f p[8] =
    {
        // Frontal Plane
       vec3f(vec2f(l, b) * h_near, -z_near),
       vec3f(vec2f(r, b) * h_near, -z_near),
       vec3f(vec2f(l, t) * h_near, -z_near),
       vec3f(vec2f(r, t) * h_near, -z_near),
	    // Back plane
       vec3f(vec2f(l, b) * h_far, -z_far),
       vec3f(vec2f(r, b) * h_far, -z_far),
       vec3f(vec2f(l, t) * h_far, -z_far),
       vec3f(vec2f(r, t) * h_far, -z_far),
    };

    // XXX It should be vectorized
    for (auto& e : p)
        e = to_world.transform3x4(e);

	_editor->drawLine(p[0], p[1]);
    _editor->drawLine(p[1], p[3]);
    _editor->drawLine(p[3], p[2]);
    _editor->drawLine(p[2], p[0]);

    _editor->drawLine(p[4], p[5]);
    _editor->drawLine(p[5], p[7]);
    _editor->drawLine(p[7], p[6]);
    _editor->drawLine(p[6], p[4]);

    _editor->drawLine(p[0], p[4]);
    _editor->drawLine(p[1], p[5]);
    _editor->drawLine(p[2], p[6]);
    _editor->drawLine(p[3], p[7]);
}

inline void
P2::renderScene()
{
    if (auto camera = Camera::current())
    {
        _renderer->setCamera(camera);
        _renderer->setImageSize(width(), height());
        _renderer->render();
        _program.use();
    }
}

inline void
P2::no_current_camera_fallback()
{
    constexpr auto title = "No current camera";

    ImGui::OpenPopup(title);
    if (ImGui::BeginPopupModal(title, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("There is no current camera selected for Renderer Mode!\n"
            "Going back to Editor Mode.\n"
            "Please, mark any camera of your choice as current.\n\n");
        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            // Fallback to Scene Editor
            _viewMode = ViewMode::Editor;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

static void
mantain_aspect(ImGuiSizeCallbackData* data)
{
    auto c = static_cast<Camera*>(data->UserData);
    data->DesiredSize.x = round(c->aspectRatio() * data->DesiredSize.y);
}

inline void
P2::preview(Camera& c)
{
    const auto MIN = ImVec2(120, 120); // pixels
    
    if (_viewMode != ViewMode::Editor)
        return;
    
    // Mantain the same aspect ratio as view window
    c.setAspectRatio(float(width()) / float(height()));
    ImGui::SetNextWindowSizeConstraints(
        MIN, ImVec2(float(width()), float(height())), mantain_aspect, &c);

    //ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    
    if (ImGui::Begin("Camera Preview"))
    {
        auto size = ImGui::GetWindowSize();
        auto pos = ImGui::GetWindowPos();
        auto y = height() - pos.y - size.y;

        // Render preview to a texture
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);

        glBindTexture(GL_TEXTURE_2D, _tex[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GLsizei(size.x), GLsizei(size.y),
            0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex[0], 0);

        glBindTexture(GL_TEXTURE_2D, _tex[1]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, GLsizei(size.x), GLsizei(size.y),
            0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _tex[1], 0);

        glViewport(0, 0, GLsizei(size.x), GLsizei(size.y));

        _renderer->setCamera(&c);
        _renderer->setImageSize(GLsizei(size.x), GLsizei(size.y));
        _renderer->render();
        _program.use();

        glViewport(0, 0, width(), height());

        // Restore default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Texture to ImGui Window
        ImGui::GetWindowDrawList()->AddImage(
            (ImTextureID) _tex[0],
            pos,
            ImVec2(size.x + pos.x, size.y + pos.y),
            ImVec2(0, 1),
            ImVec2(1, 0));

        /* Old code */
        //glEnable(GL_SCISSOR_TEST);
        //glScissor(x, y, size.x, size.y);
        //glViewport(x, y, size.x, size.y);

        //_renderer->setCamera(&c);
        //_renderer->setImageSize(size.x, size.y);
        //_renderer->render();
        //_program.use();

        //glViewport(0, 0, width(), height());
        //glDisable(GL_SCISSOR_TEST);

    }
    ImGui::End();
    
    //ImGui::PopStyleColor();
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

void
P2::render()
{
    if (_viewMode == ViewMode::Renderer)
    {
        // Fallback to scene editor if there is no current camera
        if (Camera::current())
        {
            renderScene();
            return;
        }
        
        no_current_camera_fallback();
    }
    if (_moveFlags)
    {
        const auto delta = _editor->orbitDistance() * CAMERA_RES;
        auto d = vec3f::null();

        if (_moveFlags.isSet(MoveBits::Forward))
            d.z -= delta;
        if (_moveFlags.isSet(MoveBits::Back))
            d.z += delta;
        if (_moveFlags.isSet(MoveBits::Left))
            d.x -= delta;
        if (_moveFlags.isSet(MoveBits::Right))
            d.x += delta;
        if (_moveFlags.isSet(MoveBits::Up))
            d.y += delta;
        if (_moveFlags.isSet(MoveBits::Down))
            d.y -= delta;
        _editor->pan(d);
    }
    _editor->newFrame();

    // Should it be moved to scene editor?
    auto ec = _editor->camera();
    const auto& p = ec->transform()->position();
    auto vp = vpMatrix(ec);

    _program.setUniformMat4("vpMatrix", vp);
    _program.setUniformVec4("ambientLight", _scene->ambientLight);
    _program.setUniformVec3("lightPosition", p);

    // Draw all visible primitives
    bool wireframe = _current == _scene;
    for (auto obj : _scene->get_objects())
    {
        render_scene_object(obj.get(), wireframe);
    }

    // Render a preview of the camera from the current scene object
    // if there is anyone.
    if (auto obj = dynamic_cast<SceneObject*>(_current))
    {
        // Find Camera
        Camera* camera = nullptr;
        for (auto& component : obj->get_components())
        {
            if (auto c = dynamic_cast<Camera*>(component.get()))
            {
                camera = c;
                break;
            }
        }

        if (camera)
            preview(*camera);
    }
}

void P2::render_scene_object(
    const SceneObject* obj,
    bool wireframe,
    bool only_visible)
{
    if (only_visible && !obj->visible)
        return;

    if (obj == _current)
    {
        auto t = obj->transform();
        _editor->drawAxes(t->position(), mat3f{ t->rotation() });
        //_editor->drawBounds(obj->bounds());
        wireframe = true;
    }

    for (auto component : obj->get_components())
    {
        if (auto p = dynamic_cast<Primitive*>(component.get()))
        {
            drawPrimitive(*p, wireframe);
        }
        else if (auto c = dynamic_cast<Camera*>(component.get()))
        {
            if (obj == _current)
                drawCamera(*c);
        }
    }

    // Draw children
    for (auto child : obj->get_objects())
    {
        render_scene_object(child.get(), wireframe, only_visible);
    }
}

bool
P2::windowResizeEvent(int width, int height)
{
    _editor->camera()->setAspectRatio(float(width) / float(height));
    return true;
}

bool
P2::keyInputEvent(int key, int action, int mods)
{
    auto active = action != GLFW_RELEASE && mods == GLFW_MOD_ALT;

    switch (key)
    {
    case GLFW_KEY_W:
        _moveFlags.enable(MoveBits::Forward, active);
        break;
    case GLFW_KEY_S:
        _moveFlags.enable(MoveBits::Back, active);
        break;
    case GLFW_KEY_A:
        _moveFlags.enable(MoveBits::Left, active);
        break;
    case GLFW_KEY_D:
        _moveFlags.enable(MoveBits::Right, active);
        break;
    case GLFW_KEY_Q:
        _moveFlags.enable(MoveBits::Up, active);
        break;
    case GLFW_KEY_E:
        _moveFlags.enable(MoveBits::Down, active);
        break;
    case GLFW_KEY_F:
        if (active)
            if (auto obj = dynamic_cast<SceneObject*>(_current))
                _editor->focus_on(obj->bounds());
        break;
    }
    return false;
}

bool
P2::scrollEvent(double, double yOffset)
{
    if (ImGui::GetIO().WantCaptureMouse || _viewMode == ViewMode::Renderer)
        return false;
    _editor->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
    return true;
}

bool
P2::mouseButtonInputEvent(int button, int actions, int mods)
{
    if (ImGui::GetIO().WantCaptureMouse || _viewMode == ViewMode::Renderer)
        return false;
    (void)mods;

    auto active = actions == GLFW_PRESS;

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
        _dragFlags.enable(DragBits::Rotate, active);
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        _dragFlags.enable(DragBits::Pan, active);
    if (_dragFlags)
        cursorPosition(_pivotX, _pivotY);
    return true;
}

bool
P2::mouseMoveEvent(double xPos, double yPos)
{
    if (!_dragFlags)
        return false;
    _mouseX = (int)xPos;
    _mouseY = (int)yPos;

    const auto dx = (_pivotX - _mouseX);
    const auto dy = (_pivotY - _mouseY);

    _pivotX = _mouseX;
    _pivotY = _mouseY;
    if (dx != 0 || dy != 0)
    {
        if (_dragFlags.isSet(DragBits::Rotate))
        {
            const auto da = -_editor->camera()->viewAngle() * CAMERA_RES;
            isKeyPressed(GLFW_KEY_LEFT_ALT) ?
                _editor->orbit(dy * da, dx * da) :
                _editor->rotateView(dy * da, dx * da);
        }
        if (_dragFlags.isSet(DragBits::Pan))
        {
            const auto dt = -_editor->orbitDistance() * CAMERA_RES;
            _editor->pan(-dt * math::sign(dx), dt * math::sign(dy), 0);
        }
    }
    return true;
}
