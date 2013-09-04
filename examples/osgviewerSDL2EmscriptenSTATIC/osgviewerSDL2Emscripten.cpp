/* OpenSceneGraph example, osgviewerSDL2.
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
// Simple example using SDL2 to create an OpenGL window and OSG for rendering for use with Emscripten.
// Derived from osgviewerSDL.cpp 



#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
//#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include "SDL.h"

#include <iostream>

#include <osgText/Text>
#include <osg/Geometry>

#ifdef EMSCRIPTEN_
#include <emscripten.h>
#endif
 
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
            osg::notify( osg::WARN ) << "osgwTools::transform can't transform non-Geometry yet." << std::endl;
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



bool convertEvent(SDL_Event& event, osgGA::EventQueue& eventQueue)
{
    switch (event.type) {

        case SDL_MOUSEMOTION:
            eventQueue.mouseMotion(event.motion.x, event.motion.y);
            return true;

        case SDL_MOUSEBUTTONDOWN:
            eventQueue.mouseButtonPress(event.button.x, event.button.y, event.button.button);
            return true;

        case SDL_MOUSEBUTTONUP:
            eventQueue.mouseButtonRelease(event.button.x, event.button.y, event.button.button);
            return true;

        case SDL_KEYUP:
			eventQueue.keyRelease( (osgGA::GUIEventAdapter::KeySymbol) event.key.keysym.sym);
            return true;

        case SDL_KEYDOWN:
            eventQueue.keyPress( (osgGA::GUIEventAdapter::KeySymbol) event.key.keysym.sym);
            return true;

        //case SDL_VIDEORESIZE:
        //    eventQueue.windowResize(0, 0, event.resize.w, event.resize.h );
            return true;

        default:
            break;
    }
    return false;
}



osgViewer::Viewer* viewer;

bool done;

#ifndef EMSCRIPTEN_
SDL_Window* window;
SDL_Surface *screen;
osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> gw;
#else

#endif

void mainloop()
{
	SDL_Event event;

        while ( SDL_PollEvent(&event) )
        {
            // pass the SDL event into the viewers event queue
            convertEvent(event, *(gw->getEventQueue()));

            switch (event.type) {

               /* case SDL_VIDEORESIZE:
                    SDL_SetVideoMode(event.resize.w, event.resize.h, bitDepth, SDL_OPENGL | SDL_RESIZABLE);
                    gw->resized(0, 0, event.resize.w, event.resize.h );
                    break;*/

                case SDL_KEYUP:

                    if (event.key.keysym.sym==SDLK_ESCAPE) done = true;
                    /*if (event.key.keysym.sym=='f') 
                    {
                        SDL_WM_ToggleFullScreen(screen);
                        gw->resized(0, 0, screen->w, screen->h );
                    }*/

                    break;

                case SDL_QUIT:
					delete viewer;
					viewer=0;
                    done = true;
            }
        }

        if (done) 
		{
			gw = 0;
			return;//continue;
		}

	    // draw the new frame
        viewer->frameOSG(-1.0);

        // Swap Buffers
#ifdef EMSCRIPTEN_
		SDL_GL_SwapBuffers();
#else
		SDL_GL_SwapWindow(window);
#endif //EMSCRIPTEN_
}
int main( int argc, char **argv )
{
   
    // init SDL

	
	
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);


	 SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
     SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
     SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
     SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
     SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
     SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 0);
     SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
     SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
     SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
     SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
     SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
     SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
     SDL_GL_SetAttribute(SDL_GL_STEREO, 0);
     SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
     SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
	 SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 1);


	#ifndef EMSCRIPTEN_
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_EGL, 1);
	#endif	//EMSCRIPTEN
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	

#ifdef EMSCRIPTEN_
	screen = SDL_SetVideoMode(640, 480,, 32, SDL_HWSURFACE | SDL_OPENGL);
	if ( screen == NULL )
	{
		exit(1);
	}
#else
	window = SDL_CreateWindow("ContextWindow", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL );//| SDL_WINDOW_FULLSCREEN
	// set up the surface to render to
	screen = SDL_GetWindowSurface(window);
#endif
  

    
   SDL_GLContext glcontext = SDL_GL_CreateContext(window);

   SDL_GL_SetSwapInterval(0);
   
  


     // load the scene.
    osg::ref_ptr<osg::Node> loadedModel = createScene();


   //loadedModel->setUseVertexBufferObject(true);

    if (!loadedModel)
    {
        std::cout << argv[0] <<": No data loaded." << std::endl;
        return 1;
    }

    // Starting with SDL 1.2.10, passing in 0 will use the system's current resolution.
    unsigned int windowWidth = 0;
    unsigned int windowHeight = 0;

    // Passing in 0 for bitdepth also uses the system's current bitdepth. This works before 1.2.10 too.
    unsigned int bitDepth = 0;

 
	
	// set up the surface to render to


    //if ( screen == NULL )
   // {
    //    std::cerr<<"Unable to set "<<windowWidth<<"x"<<windowHeight<<" video: %s\n"<< SDL_GetError()<<std::endl;
    //    exit(1);
    //}

    
    // If we used 0 to set the fields, query the values so we can pass it to osgViewer
    windowWidth = screen->w;
    windowHeight = screen->h;
    
	viewer = new osgViewer::Viewer();

	
    gw = viewer->setUpViewerAsEmbeddedInWindow(0,0,windowWidth,windowHeight);
	viewer->setReleaseContextAtEndOfFrameHint(false);
	viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);

    viewer->setCameraManipulator(new osgGA::TrackballManipulator);


	viewer->setSceneData( loadedModel.get());
    viewer->realize();

    done = false;
#ifdef EMSCRIPTEN_
	emscripten_set_main_loop(mainloop, 30, 0);
#else
    while( !done )
    {
        mainloop();
    }
#endif //EMSCRIPTEN_   
    return 0;
}

/*EOF*/