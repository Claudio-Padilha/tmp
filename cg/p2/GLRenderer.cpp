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
// OVERVIEW: GLRenderer.cpp
// ========
// Source file for OpenGL renderer.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 09/09/2019

#include "GLRenderer.h"

namespace cg
{ // begin namespace cg


//////////////////////////////////////////////////////////
//
// GLRenderer implementation
// ==========
void
GLRenderer::update()
{
    Renderer::update();
    // TODO
}

void
GLRenderer::render()
{
    // XXX I do not know if it should be here... But it works!
    _program.use();

    const auto& bc = _scene->backgroundColor;

    glClearColor(bc.r, bc.g, bc.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto& p = _camera->transform()->position();
    auto vp = vpMatrix(_camera);

    _program.setUniformMat4("vpMatrix", vp);
    _program.setUniformVec4("ambientLight", _scene->ambientLight);
    _program.setUniformVec3("lightPosition", p);

    // Draw all visible primitives
    for (auto it = _scene->iter_hierarchy_objects(true); it; ++it)
    {
        for (auto component : it->get_components())
        {
            if (auto p = dynamic_cast<Primitive*>(component.get()))
                drawPrimitive(*p);
        }
    }
}

inline void
GLRenderer::drawPrimitive(Primitive& p)
{
    auto m = glMesh(p.mesh());

    if (nullptr == m)
        return;

    auto t = p.transform();
    auto normalMatrix = mat3f{ t->worldToLocalMatrix() }.transposed();

    _program.setUniformMat4("transform", t->localToWorldMatrix());
    _program.setUniformMat3("normalMatrix", normalMatrix);
    _program.setUniformVec4("color", p.color);
    _program.setUniform("flatMode", (int)0);
    m->bind();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, m->vertexCount(), GL_UNSIGNED_INT, 0);
}

} // end namespace cg
