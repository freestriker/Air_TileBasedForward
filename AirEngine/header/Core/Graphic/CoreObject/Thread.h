#pragma once
#include <Utils/ThreadBase.h>
#include <qvulkanwindow.h>
#include "qloggingcategory.h"

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace CoreObject
			{
				class Thread final/* : public AirEngine::Utils::ThreadBase*/
				{
				public:
					class VulkanWindow : public QVulkanWindow
					{
					public:
						QVulkanWindowRenderer* createRenderer() override;
					};
                    class VulkanWindowRenderer : public QVulkanWindowRenderer
                    {
                    public:
						VulkanWindowRenderer(QVulkanWindow* w);

						void initResources() override;
                        void initSwapChainResources() override;
                        void releaseSwapChainResources() override;
                        void releaseResources() override;

                        void startNextFrame() override;

                    };
					static QVulkanInstance* _vulkanInstance;
					static VulkanWindow* _window;
					static VulkanWindowRenderer* _windowRenderer;
					static void Init();
					static void Start();
					static void End();
				};
			}
		}
	}
}