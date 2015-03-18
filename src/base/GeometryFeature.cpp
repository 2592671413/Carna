/*
 *  Copyright (C) 2010 - 2015 Leonid Kostrykin
 *
 *  Chair of Medical Engineering (mediTEC)
 *  RWTH Aachen University
 *  Pauwelsstr. 20
 *  52074 Aachen
 *  Germany
 *
 */

#include <Carna/base/GeometryFeature.h>
#include <Carna/base/Log.h>
#include <set>

namespace Carna
{

namespace base
{



// ----------------------------------------------------------------------------------
// GeometryFeatureLeakWatcher
// ----------------------------------------------------------------------------------

class GeometryFeatureLeakWatcher : public Singleton< GeometryFeatureLeakWatcher >
{

    NON_COPYABLE

protected:

    friend class BaseSingleton;
    GeometryFeatureLeakWatcher();

public:

    std::set< const GeometryFeature* > featureInstances;
    virtual ~GeometryFeatureLeakWatcher();
    static void reset();

}; // GeometryFeatureLeakWatcher


GeometryFeatureLeakWatcher::GeometryFeatureLeakWatcher()
{
}


GeometryFeatureLeakWatcher::~GeometryFeatureLeakWatcher()
{
    if( !featureInstances.empty() )
    {
        std::stringstream ss;
        ss << featureInstances.size() << " GeometryFeature objects leaked!";
        Log::instance().record( Log::error, ss.str() );
    }
}


void GeometryFeatureLeakWatcher::reset()
{
    Singleton< GeometryFeatureLeakWatcher >::reset();
}



// ----------------------------------------------------------------------------------
// GeometryFeature :: Details
// ----------------------------------------------------------------------------------

struct GeometryFeature::Details : public Log::OnShutdownListener
{
    Details();

    unsigned int videoResourceAcquisitions;
    std::set< Geometry* > referencingSceneGraphNodes;

    bool released;
    bool deleteIfAllowed( GeometryFeature* self );

    void logLeakedInstances() const;
    virtual void onLogShutdown() override;
};


GeometryFeature::Details::Details()
    : videoResourceAcquisitions( 0 )
    , released( false )
{
}


bool GeometryFeature::Details::deleteIfAllowed( GeometryFeature* self )
{
    if( videoResourceAcquisitions == 0 && referencingSceneGraphNodes.empty() )
    {
        delete self;
        return true;
    }
    else
    {
        return false;
    }
}


void GeometryFeature::Details::logLeakedInstances() const
{
    Log::instance().removeOnShutdownListener( *this );
    GeometryFeatureLeakWatcher::reset();
}


void GeometryFeature::Details::onLogShutdown()
{
    logLeakedInstances();
}



// ----------------------------------------------------------------------------------
// GeometryFeature :: VideoResourceAcquisition
// ----------------------------------------------------------------------------------

GeometryFeature::VideoResourceAcquisition::VideoResourceAcquisition( GeometryFeature& gf )
    : geometryFeature( gf )
{
    ++geometryFeature.pimpl->videoResourceAcquisitions;
}


GeometryFeature::VideoResourceAcquisition::~VideoResourceAcquisition()
{
    CARNA_ASSERT( geometryFeature.pimpl->videoResourceAcquisitions > 0 );
    if( --geometryFeature.pimpl->videoResourceAcquisitions == 0 )
    {
        if( geometryFeature.pimpl->released )
        {
            geometryFeature.pimpl->deleteIfAllowed( &geometryFeature );
        }
    }
}



// ----------------------------------------------------------------------------------
// GeometryFeature
// ----------------------------------------------------------------------------------

GeometryFeature::GeometryFeature()
    : pimpl( new Details() )
{
    GeometryFeatureLeakWatcher::instance().featureInstances.insert( this );
    Log::instance().addOnShutdownListener( *pimpl );
}


GeometryFeature::~GeometryFeature()
{
    GeometryFeatureLeakWatcher::instance().featureInstances.erase( this );
    if( pimpl->videoResourceAcquisitions != 0 )
    {
        Log::instance().record( Log::error, "GeometryFeature deleted while video resources still acquired!" );
    }
}


unsigned int GeometryFeature::videoResourceAcquisitionsCount() const
{
    return pimpl->videoResourceAcquisitions;
}


void GeometryFeature::release()
{
    CARNA_ASSERT( !pimpl->released );
    if( !pimpl->deleteIfAllowed( this ) )
    {
        pimpl->released = true;
    }
}


void GeometryFeature::addTo( Geometry& sceneGraphNode, unsigned int role )
{
    const std::size_t size0 = pimpl->referencingSceneGraphNodes.size();
    pimpl->referencingSceneGraphNodes.insert( &sceneGraphNode );
    if( size0 != pimpl->referencingSceneGraphNodes.size() )
    {
        sceneGraphNode.putFeature( role, *this );
    }
}


void GeometryFeature::removeFrom( Geometry& sceneGraphNode )
{
    const std::size_t size0 = pimpl->referencingSceneGraphNodes.size();
    pimpl->referencingSceneGraphNodes.erase( &sceneGraphNode );
    if( size0 != pimpl->referencingSceneGraphNodes.size() )
    {
        sceneGraphNode.removeFeature( *this );
    }
    if( pimpl->released )
    {
        pimpl->deleteIfAllowed( this );
    }
}



}  // namespace Carna :: base

}  // namespace Carna
