#ifndef VIEWER_H
#define VIEWER_H

#include "opengl.h"
#include "shader.h"
#include "camera.h"
#include "trackball.h"
#include "mesh.h"

#include <iostream>

class Viewer{
public:
    //! Constructor
    Viewer();
    virtual ~Viewer();

    // gl stuff
    void init(int w, int h);
    void drawScene();
    void drawScene2D();
    void updateAndDrawScene();
    void reshape(int w, int h);
    void loadShaders();

    // events
    void mousePressed(GLFWwindow* window, int button, int action);
    void mouseMoved(int x, int y);
    void mouseScroll(double x, double y);
    void keyPressed(int key, int action, int mods);
    void charPressed(int key);

private:
    int _winWidth, _winHeight;

    Camera _cam;
    Shader _shader;
    Mesh   _mesh;

    int _mode;
    float _zoom;
    int _angle;
    Eigen::Vector3f _translation;
    
    Eigen::Matrix4f _sun;
    Eigen::Matrix4f _earth;
    Eigen::Matrix4f _moon;
    
    // Mouse parameters for the trackball
    enum TrackMode
    {
      TM_NO_TRACK=0, TM_ROTATE_AROUND, TM_ZOOM,
      TM_LOCAL_ROTATE, TM_FLY_Z, TM_FLY_PAN
    };
    TrackMode _trackingMode = TM_NO_TRACK;
    Trackball _trackball;
    Eigen::Vector2i _lastMousePos;
};

#endif