#pragma once


#include <paf/resource.h>

#define SCE_NULL NULL

namespace paf {

	namespace graphics {
		class Texture;
		class MemoryPool;
	}

	namespace widget {
		class Widget;
	}

	class Plugin
	{
	public:

		typedef void(*PluginCB)(Plugin *plugin);

		Plugin() {};

		~Plugin() {};

		class RootWidgetInitParam
		{
		public:

			RootWidgetInitParam();

			~RootWidgetInitParam() { };

			SceInt32 unk_00;
			SceInt32 unk_04;
			SceInt32 unk_08;
			SceUChar8 unk_0C[0x20];
		};

		class TemplateInitParam
		{
		public:

			TemplateInitParam();

			~TemplateInitParam() { };

			SceInt32 unk_00;
			SceInt32 unk_04;
		};

		static Plugin *GetByName(const char *pluginName);

		static SceVoid LoadTexture(paf::graphics::Texture *tex, Plugin *plugin, paf::Resource::Element *textureSearchParam);

		paf::widget::Widget *SetRootWidget(paf::Resource::Element *widgetInfo, RootWidgetInitParam *initParam);

		paf::widget::Widget *GetRootWidgetByHash(paf::Resource::Element *widgetInfo);

		SceInt32 AddWidgetFromTemplate(paf::widget::Widget *targetRoot, paf::Resource::Element *templateSearchParam, paf::Plugin::TemplateInitParam *param);

		void *GetInterface(SceUInt32 slot);

		SceUChar8 unk_00[0x4];
		char *name;
		SceUChar8 unk_08[0xA8];
		Resource *resource;

	private:

		SceUChar8 m_unk_B4[0x4]; //size is unknown

	};

	class Framework
	{
	public:

		enum ApplicationMode
		{
			Mode_ApplicationDefault,
			Mode_Application,
			Mode_ApplicationA,
			Mode_ApplicationB,
			Mode_ApplicationC,
			Mode_ApplicationD
		};

		static Framework *s_frameworkInstance;

		class InitParam
		{
		public:

			InitParam();

			~InitParam() { };

			InitParam *LoadDefaultParams();

			SceUInt32 gxmRingBufferSize;
			SceUInt32 gxmParameterBufferSize;

			SceUInt32 unk_08;
			SceUInt32 defaultSurfacePoolSize;
			SceUInt32 graphMemSystemHeapSize;

			SceUInt32 graphMemVideoHeapSize;

			SceUInt32 textSurfaceCacheSize;

			SceUInt32 unk_1C;
			SceUInt32 unk_20;
			SceUInt32 unk_24;
			SceUInt32 unk_28;
			SceUInt32 unk_2C;
			SceUInt32 unk_30;
			SceUInt32 unk_34;
			SceUInt32 unk_38;
			SceUInt32 unk_3C;
			SceUInt32 unk_40;
			SceUInt32 unk_44;
			SceUInt32 unk_48;
			SceUInt32 unk_4C;
			SceUInt32 unk_50;

			SceUInt32 fontRasterizerFlags;
			SceUInt32 fontRasterizerHeapSize;
			SceUInt32 decodeHeapSize;
			SceUInt32 defaultPluginHeapSize;
			SceUInt32 unkSize_64;
			SceInt32 screenWidth;
			SceInt32 sceenHeight;
			ApplicationMode applicationMode;
			SceUInt32 optionalFeatureFlags;
			SceInt32 language;
			SceInt32 enterButtonAssign;
			SceUInt8 allowControlWithButtons;
			SceUInt8 unkOptFlag1;
			SceUInt8 extCtrlMode;
			SceUInt8 unkOptFlag3;
			// ui::Context *extUiContext;
			SceInt32 screenOrientationMode;
			SceInt32 graphicsFlags;

			SceUInt32 unk_90;
			SceUInt32 unk_94;
			SceUInt32 unk_98;
			SceUInt32 unk_9C;
			SceUInt32 unk_A0;
			SceUInt32 unk_A4;

			SceUInt32 audioMode;

			SceUChar8 unk2[0x8];
		};

		class CommonResourceInitParam
		{
		public:

			CommonResourceInitParam();

			~CommonResourceInitParam() { };

			paf::String resourcePath;
			SceUChar8 unk_08[0x1C];
		};

		class PluginInitParam //0x94
		{
		public:

			PluginInitParam();

			~PluginInitParam() { };

			paf::String pluginName;
			paf::String scopeName;
			paf::Plugin::PluginCB loadCB1;
			paf::Plugin::PluginCB loadCB2;
			paf::Plugin::PluginCB loadCB3;
			paf::Plugin::PluginCB unloadCB1;
			paf::Plugin::PluginCB unloadCB2;
			paf::String resourcePath;

			SceUChar8 unk_38[0x20];

			SceInt32 unk_58;

			SceUChar8 unk_5C[0xC];

			paf::String pluginPath;

			SceUChar8 unk_74[0x20];
		};

		typedef void(*LoadCRFinishCallback)();
		typedef void(*LoadPluginFinishCallback)(Plugin *plugin);
		typedef void(*UnloadPluginFinishCallback)(Plugin *plugin);

		Framework(InitParam *fwInitParam);

		~Framework();

		static SceVoid InitializeDefaultJobQueue();

		static SceUInt32 GetFwLangBySystemLang(SceUInt32 systemLanguage);

		static SceVoid LoadPluginAsync(PluginInitParam *initParam, LoadPluginFinishCallback finishCallback = SCE_NULL, UnloadPluginFinishCallback unloadFinishCallback = SCE_NULL);

		static SceVoid LoadPlugin(PluginInitParam *initParam, LoadPluginFinishCallback finishCallback = SCE_NULL, UnloadPluginFinishCallback unloadFinishCallback = SCE_NULL);

		static Framework *GetInstance();

		static paf::graphics::MemoryPool *GetDefaultGraphicsMemoryPool();

		static SceVoid LoadTexture(paf::graphics::Texture *tex, Framework *fw, paf::Resource::Element *searchParam);

		SceVoid _LoadPluginAsync(PluginInitParam *initParam, LoadPluginFinishCallback finishCallback = SCE_NULL, UnloadPluginFinishCallback unloadFinishCallback = SCE_NULL);

		SceInt32 EnterRenderingLoop();

		SceVoid ExitRenderingLoop();

		SceVoid LoadCommonResourceAsync(LoadCRFinishCallback finishCallback = SCE_NULL);

		SceVoid LoadCommonResourceAsync(CommonResourceInitParam *initParam, LoadCRFinishCallback finishCallback = SCE_NULL);

		SceVoid LoadCommonResource(LoadCRFinishCallback finishCallback = SCE_NULL);

		SceVoid LoadCommonResource(CommonResourceInitParam *initParam, LoadCRFinishCallback finishCallback = SCE_NULL);

		Plugin *FindPluginByName(const char *pluginName, SceBool enableSomeCheck = SCE_FALSE);

		ApplicationMode GetApplicationMode();

		SceInt32 Test2(SceFloat32, SceInt32, SceInt32);

		paf::widget::Widget *GetRootWidget();

		SceUChar8 unk_00[0x1C];

		Plugin *crPlugin;

		SceUChar8 unk_20[0x5C];
	};
}