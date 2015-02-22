/*
 *  Copyright (C) 2010 - 2014 Leonid Kostrykin
 *
 *  Chair of Medical Engineering (mediTEC)
 *  RWTH Aachen University
 *  Pauwelsstr. 20
 *  52074 Aachen
 *  Germany
 *
 */

#ifndef CARNA_GLOBAL_H_6014714286
#define CARNA_GLOBAL_H_6014714286

#include <QtCore/qglobal.h>

#ifdef CARNA_EXPORT
#   define CARNA_LIB Q_DECL_EXPORT
#else
#   define CARNA_LIB Q_DECL_IMPORT
#endif



// ----------------------------------------------------------------------------------
// Forward Declarations
// ----------------------------------------------------------------------------------

namespace Carna
{

    /** \brief  Contains the core framework components.
      */
    namespace base
    {
    
        class Application;
        class AssertionFailure;
        class CarnaException;
        
        /** \brief  Contains the controller components of the core framework.
          */
        namespace controller
        {
        }
        
        /** \brief  Contains the model components of the core framework.
          */
        namespace model
        {
            class GeometryDefinition;
            class HUVolume;
        }
        
        /** \brief  Contains the view components of the core framework.
          */
        namespace view
        {
            class BufferedHUVolumeTexture;
            class Camera;
            class Color;
            class FragmentShader;
            class Framebuffer;
            class FrameRenderer;
            class Geometry;
            class GLContext;
            class Node;
            class OpaqueStage;
            class Renderable;
            class RenderStage;
            class RenderTask;
            class RenderTexture;
            class Shader;
            class ShaderBundle;
            class ShaderCompilationError;
            class ShaderProgram;
            class Spatial;
            class Texture3D;
            class VertexShader;
            class Viewport;
        }
        
        /** \brief  Contains the Qt components of the core framework.
          *         These components are mostly user interface related.
          */
        namespace qt
        {
        }
        
    }

}  // namespace Carna

#endif // CARNA_GLOBAL_H_6014714286
