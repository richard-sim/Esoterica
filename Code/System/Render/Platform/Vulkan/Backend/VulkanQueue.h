#pragma once
#ifdef EE_VULKAN

namespace EE::Render
{
	namespace Backend
	{
		class VulkanQueue
		{
		public:

			enum class Type
			{
				Graphic,
				Compute,
				Transfer,
			};

			enum class Priority
			{

			};

		private:

		};
	}
}

#endif