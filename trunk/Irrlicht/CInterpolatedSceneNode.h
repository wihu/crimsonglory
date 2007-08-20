#ifndef __CINTERPOLATEDSCENENODE_H_INCLUDED__
#define __CINTERPOLATEDSCENENODE_H_INCLUDED__

#include "ISceneNode.h"

namespace irr
{
namespace scene
{
	class CInterpolatedSceneNode : public ISceneNode
	{
	public:

		//! Constructor
		CInterpolatedSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f))
			: ISceneNode(parent, mgr, id, position, rotation, scale) 
		{
			targetTranslation = position;
			targetRotation = rotation;
		}

		virtual void setRotation(const core::vector3df& rotation)
		{
			//RelativeRotation = rotation;
			targetRotation = rotation;
		}

		virtual void setPosition(const core::vector3df& newpos)
		{
			//RelativeTranslation = newpos;
			targetTranslation = newpos;
		}

	protected:
		vector3df targetTranslation, targetRotation;
	};

} // end namespace scene
} // end namespace irr

#endif

