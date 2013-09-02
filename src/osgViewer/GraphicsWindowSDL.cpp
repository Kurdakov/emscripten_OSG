#include <iostream>
#include <osgViewer/api/Win32/GraphicsWindowSDL>
#include <osgViewer/View>


using namespace osgViewer;

void GraphicsWindowSDL::init()
{
    if (_initialized)
	return;
    if (!_traits) {
        _valid = false;
        return;
    }
    if (_traits->inheritedWindowData.valid()) {
	WindowData* data
	    = dynamic_cast<WindowData*>(_traits->inheritedWindowData.get());
	if (data) {
	    _surface = data->_window;
	    _valid = true;
	    _initialized = true;
	    return;
	}
    }
    _valid = false;
}

bool GraphicsWindowSDL::realizeImplementation()
{
    if (_realized) {
        osg::notify(osg::NOTICE)<<"GraphicsWindowSDL::realizeImplementation() Already realized"<<std::endl;
        return true;
    }
    if (!_initialized)
	init();
    if (!_initialized)
	return false;
    _realized = true;
    return true;
}

bool GraphicsWindowSDL::makeCurrentImplementation()
{
    if (_realized)
	return true;
    else
	return false;
}

void GraphicsWindowSDL::closeImplementation()
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void GraphicsWindowSDL::swapBuffersImplementation()
{
    if (!_realized) return;
  //SKur  SDL_GL_SwapBuffers();
}

bool GraphicsWindowSDL::checkEvents()
{
	return true;
}

