#include "viewer.h"
#include "camera.h"

using namespace Eigen;

Viewer::Viewer()
  : _winWidth(0), _winHeight(0)
{
  _translation.x() = 0;
  _translation.y() = 0;
  _translation.z() = 0;
  _zoom = 1;
  _angle = 0;
  _mode = 0;

  Affine3f translation_sun = Translation3f(0,0,0) * Scaling(Vector3f(1,1,1));
  _sun.setIdentity();
  _sun = _sun * translation_sun.matrix();

  Affine3f translation_earth = Translation3f(3,0,0) * AngleAxisf(23.44 * M_PI / 180, Vector3f::UnitY()) * Scaling(Vector3f(0.3,0.3,0.3));
  _earth.setIdentity();
  _earth = _earth * translation_earth.matrix();

  Affine3f translation_moon = Translation3f(3.5,0,0) * AngleAxisf(23.44 * M_PI / 180, Vector3f::UnitY()) * Scaling(Vector3f(0.1,0.1,0.1));
  _moon.setIdentity();
  _moon = _moon * translation_moon.matrix();

  Vector3f t_moon = Affine3f(_earth) * Vector3f(0,0,0);
  translation_moon = Translation3f(t_moon) * AngleAxisf(5.14 * M_PI / 180,  Vector3f::UnitY()) * Translation3f(-t_moon);
  _moon = translation_moon.matrix() * _moon;
}

Viewer::~Viewer()
{
}

////////////////////////////////////////////////////////////////////////////////
// GL stuff

// initialize OpenGL context
void Viewer::init(int w, int h){
    loadShaders();

    if(!_mesh.load(DATA_DIR"/models/sphere.obj")) exit(1);
    _mesh.initVBA();

    glEnable(GL_DEPTH_TEST);

    reshape(w,h);
    _trackball.setCamera(&_cam);
    
    _cam.lookAt(Vector3f(0,0,2), Vector3f(0,0,0), Vector3f(0,1,0));
}

void Viewer::reshape(int w, int h)
{
    _winWidth = w;
    _winHeight = h;
    _cam.setViewport(w,h);
}


/*!
   callback to draw graphic primitives
 */
void Viewer::drawScene()
{
  glViewport(0, 0, _winWidth, _winHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Efface le tampon de couleur et le tampon 
  glClearColor(0.5, 0.5, 0.5, 1); // Spécifie la couleur RGBA d'effacement

  _shader.activate();

  // ===  Sun  ===
  
  glUniformMatrix4fv(_shader.getUniformLocation("transformation"), 1, GL_FALSE, _sun.data());
  glUniformMatrix4fv(_shader.getUniformLocation("view_mat"), 1, GL_FALSE, _cam.viewMatrix().data());
  glUniformMatrix4fv(_shader.getUniformLocation("proj_mat"), 1, GL_FALSE, _cam.projectionMatrix().data());
  _mesh.draw(_shader);
  
  
  // ===  Earth  ===

  // Tourne sur elle-même
  Vector3f t_earth = Affine3f(_earth) * Vector3f(0,0,0);
  Affine3f translation_earth = Translation3f(t_earth) * AngleAxisf(0.1, Vector3f::UnitY()) * Translation3f(-t_earth);
  _earth = translation_earth.matrix() * _earth;

  // Tourne autour du soleil
  translation_earth = AngleAxisf(0.01, Vector3f::UnitY());
  _earth = translation_earth.matrix() * _earth;
  
  glUniformMatrix4fv(_shader.getUniformLocation("transformation"), 1, GL_FALSE, _earth.data());
  glUniformMatrix4fv(_shader.getUniformLocation("view_mat"), 1, GL_FALSE, _cam.viewMatrix().data());
  glUniformMatrix4fv(_shader.getUniformLocation("proj_mat"), 1, GL_FALSE, _cam.projectionMatrix().data());
  _mesh.draw(_shader);

  
  // ===  Moon  ===
  
  // Tourne sur elle-même
  Vector3f t_moon = Affine3f(_moon) * Vector3f(0,0,0);
  Affine3f translation_moon = Translation3f(t_moon) * AngleAxisf(1, Vector3f::UnitY()) * Translation3f(-t_moon);
  _moon = translation_moon.matrix() * _moon;

  // Tourne autour du soleil
  translation_moon = AngleAxisf(0.02, Vector3f::UnitY());
  _moon = translation_moon.matrix() * _moon;

  // Tourne autour de la terre
  t_moon = Affine3f(_earth) * Vector3f(0,0,0);
  translation_moon = Translation3f(t_moon) * AngleAxisf(0.2, Vector3f::UnitY()) * Translation3f(-t_moon);
  _moon = translation_moon.matrix() * _moon;
  
  glUniformMatrix4fv(_shader.getUniformLocation("transformation"), 1, GL_FALSE, _moon.data());
  glUniformMatrix4fv(_shader.getUniformLocation("view_mat"), 1, GL_FALSE, _cam.viewMatrix().data());
  glUniformMatrix4fv(_shader.getUniformLocation("proj_mat"), 1, GL_FALSE, _cam.projectionMatrix().data());
  _mesh.draw(_shader);

  _shader.deactivate();
}

void Viewer::drawScene2D()
{
  glViewport(0, 0, _winWidth, _winHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Efface le tampon de couleur et le tampon 
  glClearColor(0.5, 0.5, 0.5, 1); // Spécifie la couleur RGBA d'effacement

  _shader.activate();

  Matrix4f zoom;
  zoom.setIdentity();
  zoom(0,0) *= _zoom;
  zoom(1,1) *= _zoom;

  Matrix4f rotation;
  rotation.setIdentity();
  float angle = _angle * M_PI / 180;
  rotation(0,0) = cos(angle);
  rotation(0,1) = sin(angle);
  rotation(1,0) = -sin(angle);
  rotation(1,1) = cos(angle);

  Matrix4f translation;
  translation.setIdentity();
  translation(0,3) = _translation.x();
  translation(1,3) = _translation.y();
  translation(2,3) = _translation.z();

  Matrix4f transCenterGravityBefore;
  transCenterGravityBefore.setIdentity();
  transCenterGravityBefore(0,3) = 0;
  transCenterGravityBefore(1,3) = 0.5;
  transCenterGravityBefore(2,3) = 0;

  Matrix4f transCenterGravityAfter;
  transCenterGravityAfter.setIdentity();
  transCenterGravityAfter(0,3) = 0;
  transCenterGravityAfter(1,3) = -0.5;
  transCenterGravityAfter(2,3) = 0;
  
  Matrix4f transformation = zoom * translation;
  transformation *= transCenterGravityBefore * rotation * transCenterGravityAfter;
  
  Matrix4f M1;
  M1 << 0.5,   0,   0, -0.75,
        0,   0.5,   0,    -1,
        0,   0,   0.5,     0,
        0,   0,     0,     1;
  
  Matrix4f M2;
  M2 << -0.5,   0,   0, 0.75,
           0, 0.5,   0,   -1,
           0,   0, 0.5,    0,
           0,   0,   0,    1;
  
    
  glUniformMatrix4fv(_shader.getUniformLocation("transformation"), 1, GL_FALSE, M1.data());
  _mesh.draw(_shader);

  glUniformMatrix4fv(_shader.getUniformLocation("transformation"), 1, GL_FALSE, M2.data());
  _mesh.draw(_shader);

  glUniformMatrix4fv(_shader.getUniformLocation("transformation"), 1, GL_FALSE, transformation.data());
  _mesh.draw(_shader);
  _shader.deactivate();
}

void Viewer::updateAndDrawScene()
{
    drawScene();
}

void Viewer::loadShaders()
{
    // Here we can load as many shaders as we want, currently we have only one:
    _shader.loadFromFiles(DATA_DIR"/shaders/simple.vert", DATA_DIR"/shaders/simple.frag");
    checkError();
}

////////////////////////////////////////////////////////////////////////////////
// Events

/*!
   callback to manage keyboard interactions
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::keyPressed(int key, int action, int /*mods*/)
{
  if(key == GLFW_KEY_R && action == GLFW_PRESS)
  {
    loadShaders();
  }

  if(action == GLFW_PRESS || action == GLFW_REPEAT )
  {
    
    if (key == GLFW_KEY_UP)
    {
      _translation.y() += 0.1;
    }
    else if (key == GLFW_KEY_DOWN)
    {
      _translation.y() -= 0.1;
    }
    else if (key == GLFW_KEY_LEFT)
    {
      _translation.x() -= 0.1;
    }
    else if (key == GLFW_KEY_RIGHT)
    {
      _translation.x() += 0.1;
    }
    else if (key == GLFW_KEY_PAGE_UP)
    {
      _zoom *= 2;
    }
    else if (key == GLFW_KEY_PAGE_DOWN)
    {
      _zoom *= 0.5;
    }
    else if (key == GLFW_KEY_T)
    {
      _angle += 5;
      if (_angle > 359) {
	_angle = 0;
      }
    }
    else if (key == GLFW_KEY_Y)
    {
      _angle -= 5;
      if (_angle < 0) {
	_angle = 359;
      }
    }
    else if (key == GLFW_KEY_L)
    {
      _mode++;
      if (_mode > 1) {
	_mode = 0;
      }
      
      if (_mode == 0) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      } else if (_mode == 1) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
      }
    }
  }
}

/*!
   callback to manage mouse : called when user press or release mouse button
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mousePressed(GLFWwindow */*window*/, int /*button*/, int action)
{
  if(action == GLFW_PRESS)
  {
      _trackingMode = TM_ROTATE_AROUND;
      _trackball.start();
      _trackball.track(_lastMousePos);
  }
  else if(action == GLFW_RELEASE)
  {
      _trackingMode = TM_NO_TRACK;
  }
}


/*!
   callback to manage mouse : called when user move mouse with button pressed
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mouseMoved(int x, int y)
{
    if(_trackingMode == TM_ROTATE_AROUND)
    {
        _trackball.track(Vector2i(x,y));
    }

    _lastMousePos = Vector2i(x,y);
}

void Viewer::mouseScroll(double /*x*/, double y)
{
  _cam.zoom(-0.1*y);
}

void Viewer::charPressed(int /*key*/)
{
}
