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
// OVERVIEW: SceneObject.cpp
// ========
// Source file for scene object.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 07/09/2019

#include "SceneObject.h"

#include <cassert>

#include "Scene.h"
#include "Primitive.h"

namespace cg
{ // begin namespace cg

/////////////////////////////////////////////////////////////////////
//
// SceneObject implementation
// ===========
void
    SceneObject::setParent(SceneObject* new_parent)
{
    // Assert that we are not creating a cyclic reference
    // with a scene object having its child as an ancestor
    if (new_parent && new_parent->_has_ancestor(this))
        return;

    if (new_parent == nullptr)  // Object will be part of scene root
    {
        // if there is a parent, fix parent collection
        if (_parent)
        {
            _parent->remove_object(this);
        }
        _scene->add_object(this);
    }
    else
    {
        // if there is no parent, then there is nothing to remove
        if (_parent)
            _parent->remove_object(this);
        else
            _scene->remove_object(this);

        new_parent->add_object(this);
    }

    _parent = new_parent;
    _transform.parentChanged();
}

bool
    SceneObject::_has_ancestor(const SceneObject* ancestor) const
{
    auto obj = this->parent();
    while (obj)
    {
        if (obj == ancestor)
            return true;
        obj = obj->parent();
    }

    return false;
}

Bounds3f
    SceneObject::bounds() const
{
    // Should it be optimized? Should we avoid revaluating the bounding box?
    Bounds3f my_bounds ({}, {});

    if (auto p = get<Primitive>())
        my_bounds = p->mesh()->bounds();

    my_bounds.transform(transform()->localToWorldMatrix());

    if (!_objects.empty())
    {
        for (auto& obj : _objects)
        {
            // Should we care about visible objects here?
            my_bounds.inflate(obj->bounds());
        }
    }

    return my_bounds;
}

} // end namespace cg
