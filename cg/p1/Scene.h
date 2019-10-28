//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018 Orthrus Group.                               |
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
// OVERVIEW: Scene.h
// ========
// Class definition for scene.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 25/08/2018

#ifndef __Scene_h
#define __Scene_h

#include "SceneObject.h"
#include "graphics/Color.h"

#include <list>
#include <stack>

namespace cg
{ // begin namespace cg

/////////////////////////////////////////////////////////////////////
//
// Scene: scene class
// =====
class Scene : public SceneNode
{
public:
    Color backgroundColor{ Color::gray };

    /// Constructs an empty scene.
    Scene(const char* name) :
        SceneNode{ name }
    {
        // do nothing
    }

    const auto& get_objects() const
    {
        return _objects;
    }

    const auto add_object(SceneObject* node)
    {
        return _objects.emplace_back(node);
    }

    void remove_object(SceneObject* node)
    {
        _objects.remove(node);
    }

    auto iter_hierarchy_objects(bool only_visible) const
    {
        return Iter(*this, only_visible);
    }

protected:
    std::list<SceneObject*> _objects;

public:
    class Iter
    {
    public:
        bool only_visible = false;

        Iter(const Scene& scene, bool only_visible = false)
        {
            this->only_visible = only_visible;
            push_scene_objects(scene.get_objects());
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
            // Preserve discovery order
            for (auto it = objs.rbegin(); it != objs.rend(); it++)
                if (!only_visible || (*it)->visible)
                    _remaining.push(*it);
        }
    };
}; // Scene

} // end namespace cg

#endif // __Scene_h
