//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018, 2019 Orthrus Group.                         |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: SceneObject.h
// ========
// Class definition for scene object.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 23/09/2019

#ifndef __SceneObject_h
#define __SceneObject_h

#include <algorithm>
#include <stack>
#include <vector>
#include <typeinfo>

#include "geometry/Bounds3.h"
#include "SceneNode.h"
#include "Transform.h"

namespace cg
{ // begin namespace cg

// Forward definition
class Scene;


/////////////////////////////////////////////////////////////////////
//
// SceneObject: scene object class
// ===========
class SceneObject : public SceneNode
{
public:
    bool visible{ true };

    /// Constructs an empty scene object.
    SceneObject(const char* name, Scene& scene) :
        SceneNode{ name },
        _scene{ &scene },
        _parent{}
    {
        _objects.reserve(8);
        _components.reserve(8);
        add_component(makeUse(&_transform));
    }

    /// Returns the scene which this scene object belong to.
    auto scene() const
    {
        return _scene;
    }

    /// Returns the parent of this scene object.
    auto parent() const
    {
        return _parent;
    }

    /// Sets the parent of this scene object.
    void setParent(SceneObject* parent);

    /// Returns the transform of this scene object.
    auto transform() const
    {
        return &_transform;
    }

    auto transform()
    {
        return &_transform;
    }

    const auto& get_objects() const
    {
        return _objects;
    }

    void add_object(SceneObject* obj)
    {
        _objects.emplace_back(obj);
        //_objects_changed();
    }

    void remove_object(SceneObject* obj)
    {
        auto it = std::find(_objects.cbegin(), _objects.cend(),
            Reference(obj));
        if (it != _objects.cend())
        {
            _objects.erase(it);
            //_objects_changed();
        }
    }

    const auto& get_components() const
    {
        return _components;
    }

    void add_component(Component* c)
    {
        // Do not allow two components of the same type
        // Has _components any element e of the same type as component c?
        if (std::any_of(_components.cbegin(), _components.cend(),
            [&](const Reference<Component> &e) -> bool
            { return typeid(*c) == typeid(*e.get()); }))
            return;

        c->_sceneObject = this;
        _components.push_back(c);
    }

    void remove_component(Component* c)
    {
        // Do not allow _transform to be removed
        if (c != &_transform)
            _components.erase(std::find(
                _components.cbegin(), _components.cend(), Reference(c)));
        // std::find without explicit conversion to Reference
        // does not compile
    }

    auto iter_hierarchy_objects(bool only_visible)
    {
        return Iter(*this, only_visible);
    }

    /// The bounding box of this scene object
    Bounds3f bounds() const;

    /// Returns the component of type _Derived
    /// or nullptr if it is not avaiable
    template<class _Derived>
    const _Derived* get() const
    {
        for (auto& ref : _components)
            if (auto comp = dynamic_cast<const _Derived*>(ref.get()))
                return comp;
        return nullptr;
    }

    template<class _Derived>
    _Derived* get()
    {
        for (auto& ref : _components)
            if (auto comp = dynamic_cast<_Derived*>(ref.get()))
                return comp;
        return nullptr;
    }

    /// Specialization for Transform
    template<>
    const Transform* get() const { return transform(); }

    template<>
    Transform* get() { return transform(); }

private:
    Scene* _scene;
    SceneObject* _parent;
    Transform _transform;
    //Bounds3f _bounds;
    //bool _bounds_dirty = false; // Tells us if _bounds must be revaluated
    std::vector<Reference<SceneObject>> _objects;
    std::vector<Reference<Component>> _components;

    friend class Scene;

    bool _has_ancestor(const SceneObject*) const;

    //void _objects_changed()
    //{
    //    // _bounds depends on _objects: it should be revaluated
    //    _bounds_dirty = true;
    //}

public:
    class Iter
    {
    public:
        bool only_visible = false;

        Iter(const SceneObject& scene_obj, bool only_visible = false)
        {
            this->only_visible = only_visible;
            push_scene_objects(scene_obj.get_objects());
        }

        template<class FIter>
        Iter(FIter begin, FIter end, bool only_visible = false)
        {
            this->only_visible = only_visible;
            push_scene_objects(begin, end);
        }

        bool has_next() const { return !_remaining.empty(); }

        operator bool() const { return has_next(); }

        SceneObject* get() const
        {
            return *this ? _remaining.top() : nullptr;
        }

        SceneObject* operator * () const { return get(); }

        SceneObject* operator -> () const { return get(); }

        void next()
        {
            if (has_next())
            {
                auto e = _remaining.top();
                _remaining.pop();
                push_scene_objects(e->get_objects());
            }
        }

        auto& operator ++ () { next(); return *this; }

    private:
        std::stack<SceneObject*> _remaining;

        template<class Container>
        void push_scene_objects(const Container& objs)
        {
            // Preserve discovery order using reverse iterator
            push_scene_objects(objs.rbegin(), objs.rend());
        }

        template<class FIter>
        void push_scene_objects(FIter begin, FIter end)
        {
            for (auto it = begin; it != end; it++)
                if (!only_visible || (*it)->visible)
                    _remaining.push(*it);
        }
    };

}; // SceneObject

/// Returns the transform of a component.
inline Transform*
    Component::transform() // declared in Component.h
{
    return sceneObject()->transform();
}

/// Returns the parent of a transform.
inline Transform*
    Transform::parent() const // declared in Transform.h
{
    if (auto p = sceneObject()->parent())
        return p->transform();
    return nullptr;
}

} // end namespace cg

#endif // __SceneObject_h
