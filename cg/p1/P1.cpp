#include "P1.h"

#include <algorithm>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

#include <cmath>

constexpr float g_PI = 3.14159265358979323846f;

namespace cg
{ // begin namespace cg

inline Primitive*
    makeBoxMesh()
{
    const vec4f p1{ -0.5, -0.5, -0.5, 1 };
    const vec4f p2{ +0.5, -0.5, -0.5, 1 };
    const vec4f p3{ +0.5, +0.5, -0.5, 1 };
    const vec4f p4{ -0.5, +0.5, -0.5, 1 };
    const vec4f p5{ -0.5, -0.5, +0.5, 1 };
    const vec4f p6{ +0.5, -0.5, +0.5, 1 };
    const vec4f p7{ +0.5, +0.5, +0.5, 1 };
    const vec4f p8{ -0.5, +0.5, +0.5, 1 };
    const Color c1{ Color::black };
    const Color c2{ Color::red };
    const Color c3{ Color::yellow };
    const Color c4{ Color::green };
    const Color c5{ Color::blue };
    const Color c6{ Color::magenta };
    const Color c7{ Color::cyan };
    const Color c8{ Color::white };

    // Box vertices
    static const vec4f v[]
    {
        p1, p5, p8, p4, // x = -0.5
        p2, p3, p7, p6, // x = +0.5
        p1, p2, p6, p5, // y = -0.5
        p4, p8, p7, p3, // y = +0.5
        p1, p4, p3, p2, // z = -0.5
        p5, p6, p7, p8  // z = +0.5
    };

    // Box vertex colors
    static const Color c[]
    {
        c1, c5, c8, c4, // x = -0.5
        c2, c3, c7, c6, // x = +0.5
        c1, c2, c6, c5, // y = -0.5
        c4, c8, c7, c3, // y = +0.5
        c1, c4, c3, c2, // z = -0.5
        c5, c6, c7, c8  // z = +0.5
    };

    // Box triangles
    static const GLMeshArray::Triangle t[]
    {
        { 0,  1,  2}, { 2,  3,  0},
        { 4,  5,  7}, { 5,  6,  7},
        { 8,  9, 11}, { 9, 10, 11},
        {12, 13, 14}, {14, 15, 12},
        {16, 17, 19}, {17, 18, 19},
        {20, 21, 22}, {22, 23, 20}
    };

    return new Primitive{ new GLMeshArray{24, {v, 0}, {c, 1}, 12, t} };
}

inline
std::ostream& operator << (std::ostream& out, const vec4f& v)
{
    return out << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
}

Primitive* make_sphere_mesh(int detail_level = 32)
{
	constexpr float d = 1; // Diameter
	constexpr int min_detail = 2; // Minimum detail level

	// Minimum detail level (2) -> minimum no. of layers (3)
	auto n_layers = 1 + std::max(detail_level, min_detail);
	auto vertices_per_layer = 2 * detail_level;

	// Vertex array
	std::vector<vec4f> v =
	{
		{ 0,  d / 2, 0, 1 },	// 0: top
		{ 0, -d / 2, 0, 1 }		// 1: bottom
	};
	v.reserve(2 + n_layers * vertices_per_layer);

	// Triangle array
	std::vector<GLMeshArray::Triangle> t;
	t.reserve
	(
		2 * vertices_per_layer /* top and bottom triangles */
		+ (n_layers - 3) * (vertices_per_layer - 1) /* middle triangles */
	);

	// Directions of layer vertices over xz plane
	std::vector<vec4f> xz (vertices_per_layer);

	for (int i = 0; i < vertices_per_layer; i++)
	{
		float angle = i * 2 * g_PI / n_layers;
		xz[i] = { cos(angle), 0, sin(angle), 0 };
	}

	// For each layer (except top and bottom one-vertex layers)
	for (int i = 1; i < detail_level; i++)
	{
		float y = cos(i * g_PI / n_layers);

		// Create vertices
		for (int j = 0; j < vertices_per_layer; j++)
		{
            vec4f p = xz[j];
            p.y = y;
            p = (d/2) * p.versor();
            p.w = 1;
            v.emplace_back(p);
		}

		// Start index of the vertices from current layer
		int i_1 = v.size() - vertices_per_layer;

        // Start index of the vertices from last layer
		int i_2 = i_1 - vertices_per_layer;

		// Create triangles
        if (i_2 >= 0)
        {
            for (int j = 1; j < vertices_per_layer; j++)
            {
                t.push_back({
                    i_1 + j - 1,
                    i_1 + j,
                    i_2 + j });
                t.push_back({
                    i_2 + j - 1,
                    i_1 + j - 1,
                    i_2 + j });
            }
            // Last triangles of this layer
            t.push_back({ i_1 + vertices_per_layer, i_1, i_2 });
            t.push_back({ i_2 + vertices_per_layer, i_1 - 1, i_2 });
        }
	}

    // Connect top and bottom
    auto connect_layer_to_point = [&](int i_layer, int p)
    {
        for (int j = 1; j < vertices_per_layer; j++)
        {
            t.push_back({ i_layer + j - 1, i_layer + j, p });
            //std::cerr << v[i_layer] << " to " << v[p] << "\n";
            //std::cerr << v[i_layer].length() << " to " << v[p].length() << "\n";
        }
        // Last triangle of this layer
        t.push_back({ i_layer + vertices_per_layer, i_layer, p });
    };

    // Top vertex: v[0]
    connect_layer_to_point(2, 0);
    // Bottom vertex: v[1]
    connect_layer_to_point(v.size() - vertices_per_layer, 1);

    // Colors
    std::vector<Color> c;
    c.reserve(v.size() + 16);

    while (c.size() < v.size())
    {
        c.emplace_back(Color::cyan);
        c.emplace_back(Color::gray);
        c.emplace_back(Color::white);
    }

    return new Primitive
    {
        new GLMeshArray
        {
            int(v.size()),
            {v.data(), 0},
            {c.data(), 1}, 
            int(t.size()),
            t.data()
        }
    };
}

} // end namespace cg

inline void
P1::buildScene()
{
    _current = _scene = new Scene{ "Scene 1" };

    for (auto obj : { new_sphere(), new_box(), new_empty_object() })
        add_to_current_node(obj);

    _current = add_to_current_node(new_empty_object());

    for (auto obj : { new_box(), new_box() })
        add_to_current_node(obj);

    for (auto obj : { new_box(), new_sphere(), new_empty_object() })
        add_to_current_node(obj, true);
}

void
P1::initialize()
{
    Application::loadShaders(_program, "p1.vs", "p1.fs");
    buildScene();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    glLineWidth(2.0f);
    glEnable(GL_LINE_SMOOTH);
    _program.use();
}

namespace ImGui
{
    void ShowDemoWindow(bool*);
}

SceneObject*
P1::add_to_current_node(SceneObject* obj, bool replace_current)
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
P1::hierarchyWindow()
{
    ImGui::Begin("Hierarchy");
    if (ImGui::Button("Create###object"))
        ImGui::OpenPopup("CreateObjectPopup");
    if (ImGui::BeginPopup("CreateObjectPopup"))
    {
        if (ImGui::MenuItem("Empty Object"))
        {
            auto new_obj = new_empty_object();
            if (add_to_current_node(new_obj))
            {
                // Select new object if it was added
                _current = new_obj;
            }
        }

        if (ImGui::BeginMenu("3D Object"))
        {
            if (ImGui::MenuItem("Box"))
            {
                auto new_obj = new_box();
                if (add_to_current_node(new_obj))
                {
                    // Select new object if it was added
                    _current = new_obj;
                }
            }
            if (ImGui::MenuItem("Sphere"))
            {
                auto new_obj = new_sphere();
                if (add_to_current_node(new_obj))
                {
                    // Select new object if it was added
                    _current = new_obj;
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }

    ImGui::Separator();
    
    // DFS to find all scene nodes and fill the imgui tree view with them.
    hierarchy_tree_from(_scene);

    ImGui::End();
}

void P1::hierarchy_tree_from(SceneNode* node)
{
	if (!node) return;

    ImGuiTreeNodeFlags flag { ImGuiTreeNodeFlags_OpenOnArrow };

    // Get "children" objects if there are any
    const std::list<SceneObject*>* objects = nullptr;

    // XXX Still wonder how this code would look like if the scene object collection
    // was declared in the base class.

    // Is it a scene?
    auto scene = dynamic_cast<Scene*>(node);
    if (scene)
        objects = &scene->get_objects();

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

namespace ImGui
{ // begin namespace ImGui

void
    ObjectNameInput(SceneNode* object)
{
    const int bufferSize{ 128 };
    static SceneNode* current;
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
    vec3f temp;

    temp = transform->localPosition();
    if (ImGui::DragVec3("Position", temp))
        transform->setLocalPosition(temp);
    temp = transform->localEulerAngles();
    if (ImGui::DragVec3("Rotation", temp))
        transform->setLocalEulerAngles(temp);
    temp = transform->localScale();
    if (ImGui::DragVec3("Scale", temp))
        transform->setLocalScale(temp);
}

} // end namespace ImGui

inline void
P1::sceneGui()
{
    auto scene = (Scene*)_current;

    ImGui::ObjectNameInput(_current);
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Colors"))
    {
        ImGui::ColorEdit3("Background", backgroundColor);
        ImGui::ColorEdit3("Selected Wireframe", selectedWireframeColor);
    }
}

inline void
P1::sceneObjectGui()
{
    auto object = (SceneObject*)_current;

    ImGui::ObjectNameInput(object);
    ImGui::SameLine();
    ImGui::Checkbox("###visible", &object->visible);
    ImGui::Separator();

    for (auto component : object->get_components())
    {
        if (ImGui::CollapsingHeader(component->typeName()))
        {
            // Transform
            auto t = dynamic_cast<Transform*>(component);
            if (t)
            {
                ImGui::TransformEdit(t);
                _transform = t->localToWorldMatrix();
                continue;
            }

            // Primitive
            auto p = dynamic_cast<Primitive*>(component);
            if (p)
            {
                auto m = p->mesh();
                ImGui::LabelText("vertices", "%d", m->vertexCount());
                continue;
            }
        }
    }
}

inline void
P1::objectGui()
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

inline SceneObject*
P1::new_empty_object() const
{
    static unsigned short id = 0;
    return new SceneObject {
        ("Object " + std::to_string(++id)).c_str(),
        this->_scene };
}

SceneObject* P1::new_box() const
{
    static unsigned short id = 0;
    auto box = new SceneObject {
        ("Box " + std::to_string(++id)).c_str(),
        this->_scene };
    box->add_component(makeBoxMesh());
    return box;
}

SceneObject* P1::new_sphere() const
{
	static unsigned short id = 0;
	auto sphere = new SceneObject{
		("Sphere " + std::to_string(++id)).c_str(),
		this->_scene };
	sphere->add_component(make_sphere_mesh());
	return sphere;
}

inline void
P1::inspectorWindow()
{
    ImGui::Begin("Inspector");
    objectGui();
    ImGui::End();
}

void
P1::gui()
{
    hierarchyWindow();
    inspectorWindow();
    /*
    static bool demo = true;
    ImGui::ShowDemoWindow(&demo);
    */
}

void
P1::render()
{
    GLWindow::render();
    //_program.setUniformMat4("transform", _transform);

    // Draw all visible primitives
    for (auto obj = _scene->iter_hierarchy_objects(true); obj; ++obj)
    {
        // TODO Show wireframe for all primitives in all child scene objects
        // TODO Accumulate transform down the object hierarchy
        _program.setUniformMat4("transform", obj->transform()->localToWorldMatrix());

        // From current object, iterate over its primitives
        for (auto& component : obj->get_components())
        {
            auto primitive = dynamic_cast<Primitive*>(component);
            if (!primitive) continue;
            
            // Draw primitive
            auto m = primitive->mesh();
            m->bind();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDrawElements(GL_TRIANGLES, m->vertexCount(), GL_UNSIGNED_INT, 0);

            // Is selected?
            if (_current != obj.get()) continue;

            // Draw wireframe
            m->setVertexColor(selectedWireframeColor);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, m->vertexCount(), GL_UNSIGNED_INT, 0);
            m->useVertexColors();
        }
    }
}
