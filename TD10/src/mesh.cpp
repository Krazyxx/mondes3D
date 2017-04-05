#include "mesh.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include "shader.h"
#include <Eigen/Geometry>
#include <limits>

using namespace std;
using namespace Eigen;
using namespace surface_mesh;

void Mesh::subdivide()
{
  surface_mesh::Surface_mesh newHalfEdge;

  // ==== Partie 3 ====
  Surface_mesh::Vertex_property<Point> vertices = mHalfEdge.get_vertex_property<Point>("v:point");
  Surface_mesh::Vertex_property<Surface_mesh::Vertex> vertex_mapping = mHalfEdge.add_vertex_property<Surface_mesh::Vertex>("v:mapping");
  
  Surface_mesh::Vertex_iterator vit;
  for(vit = mHalfEdge.vertices_begin(); vit != mHalfEdge.vertices_end(); ++vit) { // Parcours chaque sommets
    Surface_mesh::Halfedge he = mHalfEdge.halfedge(*vit);

    Vector3f pos = Vector3f::Zero(); // Contient la somme des positions
    float d = 0;
    do {
      pos += vertices[mHalfEdge.to_vertex(he)];
      he = mHalfEdge.next_halfedge(mHalfEdge.opposite_halfedge(he));
      d++;
    } while (he != mHalfEdge.halfedge(*vit));

    float beta = 0;
    if (beta == 3) {
      beta = 3.0/16.0;
    } else {
      beta = (1.0/d) * (5.0/8.0 - (3.0/8.0 + (1.0/4.0) * cos(2*M_PI/d)) * (3.0/8.0 + (1.0/4.0) * cos(2*M_PI/d)) ); 
    }
	
    pos = (1 - beta * d) * vertices[*vit] + beta * pos;
    vertex_mapping[*vit] = newHalfEdge.add_vertex(pos); // on rajoute les sommets initiaux repositionnés
  }
  
  
  // ==== Partie 1 ====
  Surface_mesh::Edge_property<Surface_mesh::Vertex> edge_mapping = mHalfEdge.add_edge_property<Surface_mesh::Vertex>("e:mapping");
  
  Surface_mesh::Edge_iterator eit;
  for(eit = mHalfEdge.edges_begin(); eit != mHalfEdge.edges_end(); ++eit) { // Parcours chaque arêtes
    Surface_mesh::Halfedge he = mHalfEdge.halfedge(*eit, 0);
    
    Vector3f v0 = mHalfEdge.position(mHalfEdge.to_vertex(he));
    Vector3f v1 = mHalfEdge.position(mHalfEdge.to_vertex(mHalfEdge.next_halfedge(he)));
    Vector3f v2 = mHalfEdge.position(mHalfEdge.to_vertex(mHalfEdge.opposite_halfedge(he)));
    Vector3f v3 = mHalfEdge.position(mHalfEdge.to_vertex(mHalfEdge.opposite_halfedge(mHalfEdge.next_halfedge(he))));
    
    Vector3f u1 = (3.0/8.0) * v0 + (3.0/8.0) * v2 + (1.0/8.0) * v1 + (1.0/8.0) * v3;
    
    edge_mapping[*eit] = newHalfEdge.add_vertex(u1); // On rajoute les nouveaux sommets (qui sont sur les arêtes)
  }

  
  // ==== Partie 2 ====
  Surface_mesh::Face_iterator fit;
  for (fit = mHalfEdge.faces_begin(); fit != mHalfEdge.faces_end(); ++fit) { // Parcours chaque faces
    Surface_mesh::Halfedge he = mHalfEdge.halfedge(*fit);
      
    // Find vertice first face
    Surface_mesh::Vertex v0 = vertex_mapping[mHalfEdge.to_vertex(he)];
    Surface_mesh::Vertex u1 = edge_mapping[mHalfEdge.edge(he)];
    he = mHalfEdge.next_halfedge(he);
    Surface_mesh::Vertex v3 = vertex_mapping[mHalfEdge.to_vertex(he)];
    Surface_mesh::Vertex u4 = edge_mapping[mHalfEdge.edge(he)];
    he = mHalfEdge.next_halfedge(he);
    Surface_mesh::Vertex v2 = vertex_mapping[mHalfEdge.to_vertex(he)];
    Surface_mesh::Vertex u2 = edge_mapping[mHalfEdge.edge(he)];

    // Create the new 4 faces
    newHalfEdge.add_triangle(v0,u1,u4);
    newHalfEdge.add_triangle(u4,u1,u2);
    newHalfEdge.add_triangle(u1,v2,u2);
    newHalfEdge.add_triangle(v3,u4,u2);
  }
  
  mHalfEdge = newHalfEdge;

  Mesh::updateHalfedgeToMesh();
  Mesh::updateVBO();
}

Mesh::~Mesh()
{
  if(mIsInitialized) {
    glDeleteBuffers(1,&mVertexBufferId);
    glDeleteBuffers(1,&mIndexBufferId);
    glDeleteVertexArrays(1,&mVertexArrayId);
  }
}


bool Mesh::load(const std::string& filename)
{
  std::cout << "Loading: " << filename << std::endl;

  if(!mHalfEdge.read(filename))
    return false;

  mHalfEdge.update_face_normals();
  mHalfEdge.update_vertex_normals();

  updateHalfedgeToMesh();

  return true;
}

void Mesh::updateHalfedgeToMesh()
{
  // vertex properties
  Surface_mesh::Vertex_property<Point> vertices = mHalfEdge.get_vertex_property<Point>("v:point");
  Surface_mesh::Vertex_property<Point> vnormals = mHalfEdge.get_vertex_property<Point>("v:normal");
  Surface_mesh::Vertex_property<Texture_coordinate> texcoords = mHalfEdge.get_vertex_property<Texture_coordinate>("v:texcoord");
  Surface_mesh::Vertex_property<Color> colors = mHalfEdge.get_vertex_property<Color>("v:color");

  // vertex iterator
  Surface_mesh::Vertex_iterator vit;

  Vector3f pos;
  Vector3f normal;
  Vector2f tex;
  Vector4f color;
  mVertices.clear();
  for(vit = mHalfEdge.vertices_begin(); vit != mHalfEdge.vertices_end(); ++vit)
    {
      pos = vertices[*vit];

      normal = Vector3f(0,0,0); //vnormals[*vit];

      if(texcoords)
	tex = texcoords[*vit];
      if(colors)
	color << colors[*vit], 1.0;
      else
	color = Vector4f(0.6,0.6,0.6,1.0);

      mVertices.push_back(Vertex(pos,normal,color,tex));
    }

  // face iterator
  Surface_mesh::Face_iterator fit, fend = mHalfEdge.faces_end();
  // vertex circulator
  Surface_mesh::Vertex_around_face_circulator fvit, fvend;
  Surface_mesh::Vertex v0, v1, v2;
  mFaces.clear();
  for (fit = mHalfEdge.faces_begin(); fit != fend; ++fit)
    {
      fvit = fvend = mHalfEdge.vertices(*fit);
      v0 = *fvit;
      ++fvit;
      v2 = *fvit;

      do{
	v1 = v2;
	++fvit;
	v2 = *fvit;
	mFaces.push_back(Vector3i(v0.idx(),v1.idx(),v2.idx()));
      } while (++fvit != fvend);
    }

  //updateNormals();
}

void Mesh::init()
{
  glGenVertexArrays(1,&mVertexArrayId);
  glGenBuffers(1,&mVertexBufferId);
  glGenBuffers(1,&mIndexBufferId);

  updateVBO();

  mIsInitialized = true;
}

void Mesh::updateNormals()
{
  // pass 1: set the normal to 0
  for(std::vector<Vertex>::iterator v_iter = mVertices.begin() ; v_iter!=mVertices.end() ; ++v_iter)
    v_iter->normal.setZero();

  // pass 2: compute face normals and accumulate
  for(std::size_t j=0; j<mFaces.size(); ++j)
    {
      Vector3f v0 = mVertices[mFaces[j][0]].position;
      Vector3f v1 = mVertices[mFaces[j][1]].position;
      Vector3f v2 = mVertices[mFaces[j][2]].position;

      Vector3f n = (v1-v0).cross(v2-v0).normalized();

      mVertices[mFaces[j][0]].normal += n;
      mVertices[mFaces[j][1]].normal += n;
      mVertices[mFaces[j][2]].normal += n;
    }

  // pass 3: normalize
  for(std::vector<Vertex>::iterator v_iter = mVertices.begin() ; v_iter!=mVertices.end() ; ++v_iter)
    v_iter->normal.normalize();
}

void Mesh::updateVBO()
{
  glBindVertexArray(mVertexArrayId);

  // activate the VBO:
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
  // copy the data from host's RAM to GPU's video memory:
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mVertices.size(), mVertices[0].position.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Vector3i)*mFaces.size(), mFaces[0].data(), GL_STATIC_DRAW);


}


void Mesh::draw(const Shader& shd)
{
  if (!mIsInitialized)
    init();

  // Activate the VBO of the current mesh:
  glBindVertexArray(mVertexArrayId);
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId);

  // Specify vertex data

  // 1 - get id of the attribute "vtx_position" as declared as "in vec3 vtx_position" in the vertex shader
  int vertex_loc = shd.getAttribLocation("vtx_position");
  if(vertex_loc>=0)
    {
      // 2 - tells OpenGL where to find the x, y, and z coefficients:
      glVertexAttribPointer(vertex_loc,     // id of the attribute
			    3,              // number of coefficients (here 3 for x, y, z)
			    GL_FLOAT,       // type of the coefficients (here float)
			    GL_FALSE,       // for fixed-point number types only
			    sizeof(Vertex), // number of bytes between the x coefficient of two vertices
			    // (e.g. number of bytes between x_0 and x_1)
			    0);             // number of bytes to get x_0
      // 3 - activate this stream of vertex attribute
      glEnableVertexAttribArray(vertex_loc);
    }

  int normal_loc = shd.getAttribLocation("vtx_normal");
  if(normal_loc>=0)
    {
      glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Vector3f));
      glEnableVertexAttribArray(normal_loc);
    }

  int color_loc = shd.getAttribLocation("vtx_color");
  if(color_loc>=0)
    {
      glVertexAttribPointer(color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(Vector3f)));
      glEnableVertexAttribArray(color_loc);
    }

  int texcoord_loc = shd.getAttribLocation("vtx_texcoord");
  if(texcoord_loc>=0)
    {
      glVertexAttribPointer(texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(Vector3f)+sizeof(Vector4f)));
      glEnableVertexAttribArray(texcoord_loc);
    }

  // send the geometry
  glDrawElements(GL_TRIANGLES, 3*mFaces.size(), GL_UNSIGNED_INT, 0);

  // at this point the mesh has been drawn and raserized into the framebuffer!
  if(vertex_loc>=0)   glDisableVertexAttribArray(vertex_loc);
  if(normal_loc>=0)   glDisableVertexAttribArray(normal_loc);
  if(color_loc>=0)    glDisableVertexAttribArray(color_loc);
  if(texcoord_loc>=0) glDisableVertexAttribArray(texcoord_loc);

  checkError();
}

