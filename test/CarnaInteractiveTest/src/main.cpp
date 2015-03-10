#include <QDebug>
#include <QGLWidget>
#include <QApplication>
#include <QMouseEvent>

#include <Carna/Version.h>
#include <Carna/base/Log.h>
#include <Carna/base/GLContext.h>
#include <Carna/base/FrameRenderer.h>
#include <Carna/base/Node.h>
#include <Carna/base/Camera.h>
#include <Carna/base/Geometry.h>
#include <Carna/base/GeometryFeature.h>
#include <Carna/base/BufferedHUVolumeTexture.h>
#include <Carna/base/Material.h>
#include <Carna/base/MeshFactory.h>
#include <Carna/base/Vertex.h>
#include <Carna/base/ShaderUniform.h>
#include <Carna/base/BufferedHUVolume.h>
#include <Carna/base/SpatialMovement.h>
#include <Carna/presets/OccludedRenderingStage.h>
#include <Carna/presets/OpaqueRenderingStage.h>
#include <Carna/presets/MeshColorCodingStage.h>
#include <Carna/presets/MIPStage.h>
#include <Carna/presets/MIPChannel.h>
#include <Carna/presets/DRRStage.h>
#include <Carna/presets/CuttingPlanesStage.h>
#include <Carna/helpers/HUVolumeGridHelper.h>

#include <HUGZSceneFactory.h>

#include <memory>

// ----------------------------------------------------------------------------------

CARNA_ASSERT_API_VERSION( EXPECTED_MAJOR_VERSION, EXPECTED_MINOR_VERSION, EXPECTED_RELEASE_VERSION )

// ----------------------------------------------------------------------------------



namespace Carna
{

namespace testing
{



// ----------------------------------------------------------------------------------
// QDebugLogWriter
// ----------------------------------------------------------------------------------

class QDebugLogWriter : public base::Log::TextWriter
{

protected:

    virtual void writeFormatted( base::Log::Severity, const std::string& ) const override;

};


void QDebugLogWriter::writeFormatted( base::Log::Severity severity, const std::string& msg ) const
{
    switch( severity )
    {

    case base::Log::fatal:
    case base::Log::error:
        qFatal( "%s", msg.c_str() );
        break;

    case base::Log::warning:
        qWarning( "%s", msg.c_str() );
        break;

    case base::Log::debug:
        qDebug( "%s", msg.c_str() );
        break;

    }
}



// ----------------------------------------------------------------------------------
// Demo
// ----------------------------------------------------------------------------------

class Demo : public QGLWidget
{

    const static int GEOMETRY_TYPE_VOLUMETRIC    = 0;
    const static int GEOMETRY_TYPE_OPAQUE        = 1;
    const static int GEOMETRY_TYPE_CUTTING_PLANE = 2;

    typedef Carna::helpers::HUVolumeGridHelper< Carna::base::UInt16HUVolume > GridHelper;

    std::unique_ptr< GridHelper > gridHelper;
    std::unique_ptr< base::GLContext > glContext;
    std::unique_ptr< base::FrameRenderer > renderer;
    std::unique_ptr< base::Node > root;
    base::Camera* camera;
    presets::MeshColorCodingStage* mccs;
    std::unique_ptr< base::SpatialMovement > spatialMovement;

    bool mouseInteraction;
    QPoint mousepos;

public:

    Demo();

    virtual ~Demo();

protected:

    virtual void initializeGL() override;

    virtual void resizeGL( int w, int h ) override;

    virtual void paintGL() override;
    
    virtual void mousePressEvent( QMouseEvent* ev ) override;
    
    virtual void mouseMoveEvent( QMouseEvent* ev ) override;
    
    virtual void mouseReleaseEvent( QMouseEvent* ev ) override;

}; // Demo


Demo::Demo()
    : mouseInteraction( false )
{
    setMouseTracking( true );
}


Demo::~Demo()
{
    if( renderer.get() != nullptr )
    {
        renderer->glContext().makeActive();
    }
}


void Demo::mousePressEvent( QMouseEvent* ev )
{
    mouseInteraction = true;
    const base::Aggregation< const base::Geometry > picked = mccs->pick( ev->x(), ev->y() );
    if( picked.get() == nullptr )
    {
        mousepos = ev->pos();
    }
    else
    {
        base::Geometry& pickedGeometry = const_cast< base::Geometry& >( *picked );
        spatialMovement.reset( new base::SpatialMovement( pickedGeometry, ev->x(), ev->y(), renderer->viewport(), *camera ) );
    }
    ev->accept();
}


void Demo::mouseMoveEvent( QMouseEvent* ev )
{
    const static float ROTATION_SPEED = 1e-2f;
    if( mouseInteraction )
    {
        if( spatialMovement.get() == nullptr )
        {
            const int dx = ( ev->x() - mousepos.x() );
            const int dy = ( ev->y() - mousepos.y() );
            mousepos = ev->pos();

            if( dx != 0 )
            {
                base::math::Matrix4f rotation = base::math::rotation4f( 0, 1, 0, dx * ROTATION_SPEED );
                camera->localTransform = rotation * camera->localTransform;
                updateGL();
                ev->accept();
            }
        }
        else
        {
            if( spatialMovement->update( ev->x(), ev->y() ) )
            {
                updateGL();
                ev->accept();
            }
        }
    }
}


void Demo::mouseReleaseEvent( QMouseEvent* ev )
{
    mouseInteraction = false;
    spatialMovement.reset();
    ev->accept();
}


void Demo::initializeGL()
{
    glContext.reset( new base::QGLContextAdapter< QGLContext >() );
    root.reset( new base::Node() );

    base::math::Vector3f spacing;
    std::unique_ptr< base::UInt16HUVolume > baseVolume
        ( HUGZSceneFactory::importVolume( std::string( SOURCE_PATH ) + "/../res/pelves_reduced.hugz", spacing ) );
    gridHelper.reset( new GridHelper
        ( baseVolume->size
        , baseVolume->size.x() * baseVolume->size.y() * baseVolume->size.z() * sizeof( base::UInt16HUVolume::VoxelType ) / 2 ) );
    gridHelper->loadData( *baseVolume );
    base::Node* const volumeNode = gridHelper->createNode
        ( *glContext, GEOMETRY_TYPE_VOLUMETRIC, GridHelper::Spacing( spacing ), presets::MIPStage::ROLE_HU_VOLUME );
    gridHelper->invalidateTextures( *glContext );

    base::MeshBase& boxMesh = base::MeshFactory< base::VertexBase >::createBox( 10, 10, 10 );
    base::Material& boxMaterial = base::Material::create( "unshaded" );
    boxMaterial.addUniform( new base::ShaderUniform< base::math::Vector4f >( "color", base::math::Vector4f( 1, 0, 0, 1 ) ) );
    base::Geometry* const boxGeometry = new base::Geometry( GEOMETRY_TYPE_OPAQUE );
    boxGeometry->putFeature( presets::OpaqueRenderingStage::ROLE_DEFAULT_MATERIAL, boxMaterial );
    boxGeometry->putFeature( presets::OpaqueRenderingStage::ROLE_DEFAULT_MESH, boxMesh );
    boxGeometry->localTransform = base::math::translation4f( 0, 30, 50 );

    boxMaterial.release();
    boxMesh.release();

    camera = new base::Camera();
    camera->setProjection( base::math::frustum4f( 3.14f * 45 / 180.f, 1, 10, 2000 ) );
    camera->localTransform = base::math::translation4f( 0, 0, 500 );
    root->attachChild( camera );
    root->attachChild( volumeNode );
    root->attachChild( boxGeometry );

    base::Geometry* const plane1 = new base::Geometry( GEOMETRY_TYPE_CUTTING_PLANE );
    plane1->localTransform = base::math::plane4f( base::math::Vector3f( 1, 1, 1 ).normalized(), 0 );
    root->attachChild( plane1 );
}


void Demo::resizeGL( int w, int h )
{
    const static bool fitSquare = true;
    if( renderer.get() == nullptr )
    {
        renderer.reset( new base::FrameRenderer( *glContext, static_cast< unsigned >( w ), static_cast< unsigned >( h ), fitSquare ) );

        /* Picking
         */
        mccs = new presets::MeshColorCodingStage();
        mccs->putGeometryType( GEOMETRY_TYPE_OPAQUE, presets::OpaqueRenderingStage::ROLE_DEFAULT_MESH );
        renderer->appendStage( mccs );

        /* Cutting Planes
         */
        presets::CuttingPlanesStage* const cuttingPlanes
            = new presets::CuttingPlanesStage( GEOMETRY_TYPE_VOLUMETRIC, GEOMETRY_TYPE_CUTTING_PLANE );
        cuttingPlanes->setWindowingWidth( 1000 );
        cuttingPlanes->setWindowingLevel( -100 );
        renderer->appendStage( cuttingPlanes );

        /* Occluded Renderer
         */
        presets::OccludedRenderingStage* const occluded = new presets::OccludedRenderingStage();
        renderer->appendStage( occluded );

        /* Opaque
         */
        presets::OpaqueRenderingStage* const opaque = new presets::OpaqueRenderingStage( GEOMETRY_TYPE_OPAQUE );
        renderer->appendStage( opaque );
        occluded->enableStage( *opaque );
#if 0
        /* MIP
         */
        presets::MIPStage* const mip = new presets::MIPStage( GEOMETRY_TYPE_VOLUMETRIC );
        mip->appendChannel( new presets::MIPChannel( -1024, 0, base::math::Vector4f( 0, 0, 1, 0.5f ) ) );
        mip->appendChannel( new presets::MIPChannel( 0, 3071, base::math::Vector4f( 1, 1, 0, 0.5f ) ) );
        renderer->appendStage( mip );
#else
        /* DRR
         */
        presets::DRRStage* const drr = new presets::DRRStage( GEOMETRY_TYPE_VOLUMETRIC );
        renderer->appendStage( drr );
#endif
    }
    else
    {
        renderer->reshape( static_cast< unsigned >( w ), static_cast< unsigned >( h ), fitSquare );
    }
}


void Demo::paintGL()
{
    renderer->render( *camera, *root );
}


}  // namespace Carna :: testing

}  // namespace Carna



// ----------------------------------------------------------------------------------
// main
// ----------------------------------------------------------------------------------

int main( int argc, char** argv )
{
    Carna::base::Log::instance().setWriter( new Carna::testing::QDebugLogWriter() );
    QApplication app( argc, argv );
    Carna::testing::Demo demo;
    //demo.resize( 400, 400 );
    demo.show();
    return QApplication::exec();
}
