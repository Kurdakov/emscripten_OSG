/* OpenSceneGraph example, osgviewerGlut.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

// (C) 2005 Mike Weiblen http://mew.cx/ released under the OSGPL.
// Simple example using GLUT to create an OpenGL window and OSG for rendering.
// Derived from osgGLUTsimple.cpp and osgkeyboardmouse.cpp

// (C) 2013 Sergey Kurdakov  https://github.com/Kurdakov/emscripten_OSG/wiki/Introduction released under the OSGPL.
// Simple example using GLUT to create an OpenGL window and OSG for rendering  with Emscripten.
// Derived from osgviewerSDL.cpp 
// Cube code is borrowed  from osgwTools

#include <osg/Config>

#if defined(_MSC_VER) && defined(OSG_DISABLE_MSVC_WARNINGS)
    // disable warning "glutCreateMenu_ATEXIT_HACK' : unreferenced local function has been removed"
    #pragma warning( disable : 4505 )
#endif

#include <iostream>
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <esGLUT/esGLUT.h>
#endif

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>

osg::ref_ptr<osgViewer::Viewer>  viewer;
osg::observer_ptr<osgViewer::GraphicsWindow> window;


static const char gVertexShader[] =
    "varying vec4 color;                                                    \n"
    "const vec3 lightPos      =vec3(0.0, 0.0, 10.0);                        \n"
    "const vec4 cessnaColor   =vec4(0.8, 0.8, 0.8, 1.0);                    \n"
    "const vec4 lightAmbient  =vec4(0.1, 0.1, 0.1, 1.0);                    \n"
    "const vec4 lightDiffuse  =vec4(0.4, 0.4, 0.4, 1.0);                    \n"
    "const vec4 lightSpecular =vec4(0.8, 0.8, 0.8, 1.0);                    \n"
    "void DirectionalLight(in vec3 normal,                                  \n"
    "                      in vec3 ecPos,                                   \n"
    "                      inout vec4 ambient,                              \n"
    "                      inout vec4 diffuse,                              \n"
    "                      inout vec4 specular)                             \n"
    "{                                                                      \n"
    "     float nDotVP;                                                     \n"
    "     vec3 L = normalize(gl_ModelViewMatrix*vec4(lightPos, 0.0)).xyz;   \n"
    "     nDotVP = max(0.0, dot(normal, L));                                \n"
    "                                                                       \n"
    "     if (nDotVP > 0.0) {                                               \n"
    "       vec3 E = normalize(-ecPos);                                     \n"
    "       vec3 R = normalize(reflect( L, normal ));                       \n"
    "       specular = pow(max(dot(R, E), 0.0), 16.0) * lightSpecular;      \n"
    "     }                                                                 \n"
    "     ambient  = lightAmbient;                                          \n"
    "     diffuse  = lightDiffuse * nDotVP;                                 \n"
    "}                                                                      \n"
    "void main() {                                                          \n"
    "    vec4 ambiCol = vec4(0.0);                                          \n"
    "    vec4 diffCol = vec4(0.0);                                          \n"
    "    vec4 specCol = vec4(0.0);                                          \n"
    "    gl_Position   = gl_ModelViewProjectionMatrix * gl_Vertex;          \n"
    "    vec3 normal   = normalize(gl_NormalMatrix * gl_Normal);            \n"
    "    vec4 ecPos    = gl_ModelViewMatrix * gl_Vertex;                    \n"
    "    DirectionalLight(normal, ecPos.xyz, ambiCol, diffCol, specCol);    \n"
    "    color = cessnaColor * (ambiCol + diffCol + specCol);               \n"
    "}                                                                      \n";

static const char gFragmentShader[] =
    "precision mediump float;                  \n"
    "varying mediump vec4 color;               \n"
    "void main() {                             \n"
    "  gl_FragColor = color;                   \n"
    "}                                         \n";


				   
																						   
																						   
																						   
																						   
																						   
																						   
																						   



//box
static bool
buildPlainBoxData( const osg::Vec3& halfExtents, osg::Geometry* geom )
{
    const float xMin( -halfExtents[ 0 ] );
    const float xMax( halfExtents[ 0 ] );
    const float yMin( -halfExtents[ 1 ] );
    const float yMax( halfExtents[ 1 ] );
    const float zMin( -halfExtents[ 2 ] );
    const float zMax( halfExtents[ 2 ] );

    geom->setNormalBinding( osg::Geometry::BIND_OFF );
    geom->setColorBinding( osg::Geometry::BIND_OFF );

    osg::Vec3Array* v = new osg::Vec3Array;
    geom->setVertexArray( v );
    v->push_back( osg::Vec3( xMin, yMax, zMin ) );
    v->push_back( osg::Vec3( xMax, yMax, zMin ) );
    v->push_back( osg::Vec3( xMin, yMin, zMin ) );
    v->push_back( osg::Vec3( xMax, yMin, zMin ) );
    v->push_back( osg::Vec3( xMin, yMin, zMax ) );
    v->push_back( osg::Vec3( xMax, yMin, zMax ) );
    v->push_back( osg::Vec3( xMin, yMax, zMax ) );
    v->push_back( osg::Vec3( xMax, yMax, zMax ) );

    osg::DrawElementsUInt* deui = new osg::DrawElementsUInt( GL_TRIANGLE_STRIP );
    for( unsigned int idx = 0; idx<8; idx++ )
        deui->push_back( idx );
    deui->push_back( 0 );
    deui->push_back( 1 );
    geom->addPrimitiveSet( deui );

    deui = new osg::DrawElementsUInt( GL_TRIANGLE_STRIP );
    deui->push_back( 0 );
    deui->push_back( 2 );
    deui->push_back( 6 );
    deui->push_back( 4 );
    geom->addPrimitiveSet( deui );

    deui = new osg::DrawElementsUInt( GL_TRIANGLE_STRIP );
    deui->push_back( 1 );
    deui->push_back( 7 );
    deui->push_back( 3 );
    deui->push_back( 5 );
    geom->addPrimitiveSet( deui );

    return( true );
}


osg::Geometry*
makePlainBox( const osg::Vec3& halfExtents, osg::Geometry* geometry = NULL )
{
    osg::ref_ptr< osg::Geometry > geom( geometry );
    if( geom == NULL )
        geom = new osg::Geometry;

    bool result = buildPlainBoxData( halfExtents, geom.get() );
    if( !result )
    {
        osg::notify( osg::WARN ) << "makeBox: Error during box build." << std::endl;
        return( NULL );
    }
    else
        return( geom.release() );
}

///
void transform( const osg::Matrix& m, osg::Vec3Array* verts, bool normalize =false )
{
    osg::Vec3Array::iterator itr;
    for( itr = verts->begin(); itr != verts->end(); itr++ )
    {
        *itr = *itr * m;
        if( normalize )
            itr->normalize();
    }
    verts->dirty();
}

osg::BoundingSphere transform( const osg::Matrix& m, const osg::BoundingSphere& sphere )
{
    osg::BoundingSphere::vec_type xdash = sphere._center;
    xdash.x() += sphere._radius;
    xdash = xdash * m;

    osg::BoundingSphere::vec_type ydash = sphere._center;
    ydash.y() += sphere._radius;
    ydash = ydash * m;

    osg::BoundingSphere::vec_type zdash = sphere._center;
    zdash.z() += sphere._radius;
    zdash = zdash * m;

    osg::BoundingSphere newSphere;
    newSphere._center = sphere._center * m;

    xdash -= newSphere._center;
    osg::BoundingSphere::value_type len_xdash = xdash.length();

    ydash -= newSphere._center;
    osg::BoundingSphere::value_type len_ydash = ydash.length();

    zdash -= newSphere._center;
    osg::BoundingSphere::value_type len_zdash = zdash.length();

    newSphere._radius = len_xdash;
    if( newSphere._radius < len_ydash )
        newSphere._radius = len_ydash;
    if( newSphere._radius < len_zdash )
        newSphere._radius = len_zdash;

    return( newSphere );
}

osg::BoundingBox transform( const osg::Matrix& m, const osg::BoundingBox& box )
{
    osg::ref_ptr< osg::Vec3Array > v = new osg::Vec3Array;
    v->resize( 8 );
    (*v)[0].set( box._min );
    (*v)[1].set( box._max.x(), box._min.y(), box._min.z() );
    (*v)[2].set( box._max.x(), box._min.y(), box._max.z() );
    (*v)[3].set( box._min.x(), box._min.y(), box._max.z() );
    (*v)[4].set( box._max );
    (*v)[5].set( box._min.x(), box._max.y(), box._max.z() );
    (*v)[6].set( box._min.x(), box._max.y(), box._min.z() );
    (*v)[7].set( box._max.x(), box._max.y(), box._min.z() );

    transform( m, v.get() );

    osg::BoundingBox newBox( (*v)[0], (*v)[1] );
    newBox.expandBy( (*v)[2] );
    newBox.expandBy( (*v)[3] );
    newBox.expandBy( (*v)[4] );
    newBox.expandBy( (*v)[5] );
    newBox.expandBy( (*v)[6] );
    newBox.expandBy( (*v)[7] );

    return( newBox );
}


void transform( const osg::Matrix& m, osg::Geometry* geom )
{
    if( geom == NULL )
        return;

    // Transform the vertices by the specified matrix.
    osg::Vec3Array* v = dynamic_cast< osg::Vec3Array* >( geom->getVertexArray() );
    if( v != NULL )
    {
        transform( m, v );
    }

    // Transform the normals by the upper-left 3x3 matrix (no translation)
    // and rescale the normals in case the matrix contains scalng.
    osg::Vec3Array* n = dynamic_cast< osg::Vec3Array* >( geom->getNormalArray() );
    if( n != NULL )
    {
        osg::Matrix m3x3( m );
        m3x3.setTrans( 0., 0., 0. );
        transform( m3x3, n, true );
    }

    geom->dirtyDisplayList();
    geom->dirtyBound();
}

void transform( const osg::Matrix& m, osg::Geode* geode )
{
    if( geode == NULL )
        return;

    unsigned int idx;
    for( idx=0; idx<geode->getNumDrawables(); idx++ )
    {
        osg::Geometry* geom = geode->getDrawable( idx )->asGeometry();
        if( geom != NULL )
        {
            transform( m, geom );
        }
        else
        {
            // TBD need support for OSG shape drawables.
            osg::notify( osg::WARN ) << "transform can't transform non-Geometry yet." << std::endl;
        }
    }
}

///

osg::Geometry*
makePlainBox( const osg::Matrix& m, const osg::Vec3& halfExtents, osg::Geometry* geometry )
{
    osg::Geometry* geom = makePlainBox( halfExtents, geometry );
    if( geom != NULL )
        transform( m, geom );
    return( geom );
}





osg::Geode* createScene() {
    osg::Geode*   geode   = new osg::Geode();
    osg::Program* program = new osg::Program();

    osg::Shader*  vert    = new osg::Shader(osg::Shader::VERTEX, gVertexShader);
    osg::Shader*  frag    =  new osg::Shader(osg::Shader::FRAGMENT, gFragmentShader );

    program->addShader(vert);
    program->addShader(frag);


    //geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(), 3.0)));
	osg::Vec3 vec(1.,1.,1.);
	geode->addDrawable(makePlainBox (vec));
    geode->getOrCreateStateSet()->setAttribute(program);

    return geode;
}


void display(void)
{
    // update and render the scene graph
    if (viewer.valid()) viewer->frameOSG(-1);

    // Swap Buffers
    glutSwapBuffers();
    glutPostRedisplay();
}

void reshape( int w, int h )
{
    // update the window dimensions, in case the window has been resized.
    if (window.valid()) 
    {
        window->resized(window->getTraits()->x, window->getTraits()->y, w, h);
        window->getEventQueue()->windowResize(window->getTraits()->x, window->getTraits()->y, w, h );
    }
}

void mousebutton( int button, int state, int x, int y )
{
    if (window.valid())
    {
        if (state==0) window->getEventQueue()->mouseButtonPress( x, y, button+1 );
        else window->getEventQueue()->mouseButtonRelease( x, y, button+1 );
    }
}

void mousemove( int x, int y )
{
    if (window.valid())
    {
        window->getEventQueue()->mouseMotion( x, y );
    }
}

void keyboard( unsigned char key, int /*x*/, int /*y*/ )
{
    switch( key )
    {
        case 27:
            // clean up the viewer 
            if (viewer.valid()) 
			{
				window = 0;
				viewer = 0;
			}
            //glutDestroyWindow(glutGetWindow());
            break;
        default:
            if (window.valid())
            {
                window->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) key );
                window->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) key );
            }
            break;
    }
}

int main( int argc, char **argv )
{
    glutInit(&argc, argv);


    // load the scene.
    osg::ref_ptr<osg::Node> loadedModel = createScene();
    if (!loadedModel)
    {
        std::cout << argv[0] <<": No data loaded." << std::endl;
        return 1;
    }

    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH /* | GLUT_ALPHA */);
    //glutInitWindowPosition( 100, 100 );
    glutInitWindowSize( 800, 600 );
    glutCreateWindow( argv[0] );
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutMouseFunc( mousebutton );
    glutMotionFunc( mousemove );
    glutKeyboardFunc( keyboard );

    // create the view of the scene.
    viewer = new osgViewer::Viewer;
    window = viewer->setUpViewerAsEmbeddedInWindow(100,100,800,600);
    viewer->setSceneData(loadedModel.get());
    viewer->setCameraManipulator(new osgGA::TrackballManipulator);
    //viewer->addEventHandler(new osgViewer::StatsHandler);
    viewer->realize();

    glutMainLoop();


	window= 0;
	viewer = 0;
	//
    
    return 0;
}

/*EOF*/
