#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/TriMesh.h"
#include "cinder/MayaCamUI.h"

#define OM_STATIC_BUILD

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/CatmullClarkT.hh>

using namespace ci;
using namespace ci::app;
using namespace std;

typedef OpenMesh::PolyMesh_ArrayKernelT<>  Mesh;

class SubdivideTestApp : public AppNative {
public:
	void setup();
	void update();
	void draw();
    
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
    
    TriMesh         mTriMesh;
    MayaCamUI       mMayaCam;
};

void SubdivideTestApp::setup()
{
    // Setup Camera
    CameraPersp cam = CameraPersp(getWindowWidth(), getWindowHeight(), 60, 2.0f, 5000.0f);
	cam.setEyePoint( Vec3f( 69.5013f, 22.654f, 232.903f ) );
    cam.setWorldUp( Vec3f( 0, -1, 0 ) );
    cam.setCenterOfInterestPoint( Vec3f( 0.0f, 0.0f, 0.0f ) );
    
    mMayaCam.setCurrentCam( cam );
    
    Mesh mesh;
    
    // generate vertices
    
    Mesh::VertexHandle vhandle[8];
    
    float cubeScale = 50.0f;
    vhandle[0] = mesh.add_vertex(Mesh::Point(-1, -1,  1)*cubeScale);
    vhandle[1] = mesh.add_vertex(Mesh::Point( 1, -1,  1)*cubeScale);
    vhandle[2] = mesh.add_vertex(Mesh::Point( 1,  1,  1)*cubeScale);
    vhandle[3] = mesh.add_vertex(Mesh::Point(-1,  1,  1)*cubeScale);
    vhandle[4] = mesh.add_vertex(Mesh::Point(-1, -1, -1)*cubeScale);
    vhandle[5] = mesh.add_vertex(Mesh::Point( 1, -1, -1)*cubeScale);
    vhandle[6] = mesh.add_vertex(Mesh::Point( 1,  1, -1)*cubeScale);
    vhandle[7] = mesh.add_vertex(Mesh::Point(-1,  1, -1)*cubeScale);
    
    
    // generate (quadrilateral) faces
    
    std::vector<Mesh::VertexHandle>  face_vhandles;
    
    face_vhandles.clear();
    face_vhandles.push_back(vhandle[0]);
    face_vhandles.push_back(vhandle[1]);
    face_vhandles.push_back(vhandle[2]);
    face_vhandles.push_back(vhandle[3]);
    mesh.add_face(face_vhandles);
    
    face_vhandles.clear();
    face_vhandles.push_back(vhandle[7]);
    face_vhandles.push_back(vhandle[6]);
    face_vhandles.push_back(vhandle[5]);
    face_vhandles.push_back(vhandle[4]);
    mesh.add_face(face_vhandles);
    
    face_vhandles.clear();
    face_vhandles.push_back(vhandle[1]);
    face_vhandles.push_back(vhandle[0]);
    face_vhandles.push_back(vhandle[4]);
    face_vhandles.push_back(vhandle[5]);
    mesh.add_face(face_vhandles);
    
    face_vhandles.clear();
    face_vhandles.push_back(vhandle[2]);
    face_vhandles.push_back(vhandle[1]);
    face_vhandles.push_back(vhandle[5]);
    face_vhandles.push_back(vhandle[6]);
    mesh.add_face(face_vhandles);
    
    face_vhandles.clear();
    face_vhandles.push_back(vhandle[3]);
    face_vhandles.push_back(vhandle[2]);
    face_vhandles.push_back(vhandle[6]);
    face_vhandles.push_back(vhandle[7]);
    mesh.add_face(face_vhandles);
    
    face_vhandles.clear();
    face_vhandles.push_back(vhandle[0]);
    face_vhandles.push_back(vhandle[3]);
    face_vhandles.push_back(vhandle[7]);
    face_vhandles.push_back(vhandle[4]);
    mesh.add_face(face_vhandles);
    
    
    // Subdivide 6 times
    OpenMesh::Subdivider::Uniform::CatmullClarkT<Mesh> catmull;
    
    catmull.attach(mesh);
    catmull( 6 );
    catmull.detach();
    
    // triangulate faces
    mesh.triangulate();
    
    // Convert to Cinder TriMesh
    for( Mesh::ConstVertexIter vertex = mesh.vertices_begin(); vertex != mesh.vertices_end(); ++vertex ){
        Mesh::Point  point   = mesh.point( vertex.handle() );
        mTriMesh.appendVertex( Vec3f( point[0], point[1], point[2] ) );
    }
    
    for( Mesh::ConstFaceIter face = mesh.faces_begin(); face != mesh.faces_end(); ++face ){
        vector<uint32_t> indices;
        for( Mesh::ConstFaceVertexIter faceVertex = mesh.cfv_iter( face.handle() ); faceVertex; ++faceVertex ){
            indices.push_back( faceVertex.handle().idx() );
        }
        
        mTriMesh.appendTriangle( indices[0], indices[1], indices[2] );
    }
}

void SubdivideTestApp::update()
{
}

void SubdivideTestApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::setMatrices( mMayaCam.getCamera() );
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    gl::color( Color::black() );
    gl::draw( mTriMesh );
    
    gl::color( Color::white() );
    gl::enableWireframe();
    gl::draw( mTriMesh );
    gl::disableWireframe();
}

void SubdivideTestApp::mouseDown( MouseEvent event )
{
    mMayaCam.mouseDown( event.getPos() );
}

void SubdivideTestApp::mouseDrag( MouseEvent event )
{
    mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}
CINDER_APP_NATIVE( SubdivideTestApp, RendererGl )
