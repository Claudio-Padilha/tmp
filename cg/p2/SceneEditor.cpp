//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019 Orthrus Group.                               |
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
// OVERVIEW: SceneEditor.cpp
// ========
// Source file for scene editor.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 23/09/2019

#include "SceneEditor.h"

#include <algorithm>

namespace cg
{ // begin namespace cg

/////////////////////////////////////////////////////////////////////
//
// SceneEditor impementation
// ===========
void
SceneEditor::setDefaultView(float aspect)
{
  auto t = _camera->transform();

  t->reset();
  t->setLocalPosition({0, 0, 10});
  _camera->reset(aspect);
}

void SceneEditor::focus_on(Bounds3f bounds)
{
    // Everything here is relative to the World Coordinates
    // v_ stands for a vector
    // p_ stands for a point

    // Direction of Projection
    auto v_dop = -_camera->transform()->forward();

    // Distance that the camera should stay away
    // from the center of the bounds.
    auto distance = _camera->z_near();

    // Determining camera distance based on projection type
    switch (_camera->projectionType())
    {
    case Camera::Parallel:
        // Preserve original height provided that the entire object is visible
        _camera->setHeight(std::max(bounds.size().y, _camera->height()));
        // Preserve original distance provided that the projection plane
        // does not slice the target object
        distance = std::max(
            bounds.diagonalLength(),
            (bounds.center() - _camera->transform()->position()).length());
        break;
    case Camera::Perspective:
        // F = \frac{H}{2 \tan \frac{\theta}{2}}
        distance += 0.5f * bounds.diagonalLength()
            / tanf(math::toRadians(_camera->viewAngle() / 2.f));
        break;
    }

    // Point where the camera must be delivered
    auto p_final = bounds.center() - distance * v_dop;

    // Set camera position
    _camera->transform()->setPosition(p_final);
}

void
SceneEditor::zoom(float s)
{
  if (s > 0)
    if (_camera->projectionType() == Camera::Perspective)
      _camera->setViewAngle(_camera->viewAngle() / s);
    else
      _camera->setHeight(_camera->height() / s);
}

void
SceneEditor::rotateView(float ax, float ay)
{
  auto t = _camera->transform();
  auto q = quatf{ay, vec3f{0, 1, 0}} * t->rotation();

  t->setRotation(quatf{ax, q.rotate({1, 0, 0})} * q);
}

void
SceneEditor::orbit(float ax, float ay)
{
  auto t = _camera->transform();
  auto p = t->position() - t->forward() * _orbitDistance;

  rotateView(ax, ay);
  t->setPosition(p + t->forward() * _orbitDistance);
}

void
SceneEditor::pan(const vec3f& d)
{
  _camera->transform()->translate(d, Transform::Space::Local);
}

void
SceneEditor::newFrame()
{
  const auto& bc = _scene->backgroundColor;

  glClearColor(bc.r, bc.g, bc.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  setView(_camera->transform()->position(), vpMatrix(_camera));
  if (showGround)
    drawXZPlane(10, 1);
}

} // end namespace cg
