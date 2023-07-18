#include "RenderPipelineState.h"

namespace EE
{
	namespace Render
	{
		PipelineShaderDesc::PipelineShaderDesc( PipelineStage stage, ResourcePath shaderPath, String entryName )
			: m_stage( stage ), m_shaderPath( shaderPath ), m_entryName( entryName )
		{
			EE_ASSERT( m_shaderPath.IsValid() && m_shaderPath.IsFile() );
		}
	}
}