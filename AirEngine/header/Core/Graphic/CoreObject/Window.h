#pragma once
#include <Utils/ThreadBase.h>
#include <qvulkanwindow.h>
#include <qloggingcategory.h>
#include "qvulkanfunctions.h"
namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace CoreObject
			{
				class Window final
				{
				public:
					class VulkanWindow : public QVulkanWindow
					{
					private:
						void keyReleaseEvent(QKeyEvent* keyEvent)override;
						void keyPressEvent(QKeyEvent* keyEvent)override;
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
					static QVulkanInstance* _qVulkanInstance;
					static VulkanWindow* _window;
					static VulkanWindowRenderer* _windowRenderer;
					static QVulkanInstance* QVulkanInstance_();
					static VulkanWindow* VulkanWindow_();
					static VulkanWindowRenderer* VulkanWindowRenderer_();
					static VkExtent2D Extent();
					static void Init();
					static void Start();
					static void End();
				};
			}
		}
	}
}