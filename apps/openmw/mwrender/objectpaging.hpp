#ifndef OPENMW_COMPONENTS_ESMPAGING_CHUNKMANAGER_H
#define OPENMW_COMPONENTS_ESMPAGING_CHUNKMANAGER_H

#include <components/terrain/quadtreeworld.hpp>
#include <components/resource/resourcemanager.hpp>
#include <components/esm/loadcell.hpp>

#include <OpenThreads/ReadWriteMutex>

namespace Resource
{
    class SceneManager;
}
namespace MWWorld
{
    class ESMStore;
}

namespace MWRender
{

    typedef std::tuple<osg::Vec2f, float> ChunkId; // Center, Size

    class ObjectPaging : public Resource::GenericResourceManager<ChunkId>, public Terrain::QuadTreeWorld::ChunkManager
    {
    public:
        ObjectPaging(Resource::SceneManager* sceneManager);
        ~ObjectPaging() = default;

        osg::ref_ptr<osg::Node> getChunk(float size, const osg::Vec2f& center, unsigned char lod, unsigned int lodFlags, bool far, const osg::Vec3f& viewPoint, bool compile) override;

        osg::ref_ptr<osg::Node> createChunk(float size, const osg::Vec2f& center, const osg::Vec3f& viewPoint, bool compile);

        void enableObject(const ESM::RefNum & refnum, bool enabled);
        void clear();

        void reportStats(unsigned int frameNumber, osg::Stats* stats) const override;

    private:
        Resource::SceneManager* mSceneManager;
        float mMergeFactor;
        float mMinSize;

        OpenThreads::ReadWriteMutex mDisabledMutex;
        std::set<ESM::RefNum> mDisabled;
    };

}

#endif