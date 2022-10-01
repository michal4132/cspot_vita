#pragma once

#include <paf/resource.h>


namespace paf {

	class WString;

	namespace graphics {
		class Texture;
	}

	namespace widget {

		class Widget
		{
		public:

			enum Error //placeholder names
			{
				SCE_PAF_ERROR_UI_WIDGET_COLOR_ERROR = 0x80AF0022
			};

			class EventCallback
			{
			public:

				enum Error
				{
					SCE_PAF_ERROR_UI_WIDGET_EVENT_CALLBACK_UNHANDLED = 0x80AF4101
				};

				typedef void(*EventHandler)(SceInt32 eventId, Widget *self, SceInt32, ScePVoid pUserData);

				EventCallback() : state(2), pUserData(NULL)
				{

				};

				virtual ~EventCallback()
				{

				};

				virtual SceInt32 HandleEvent(SceInt32 eventId, Widget *self, SceInt32 a3)
				{
					SceInt32 ret;

					if ((this->state & 1) == 0) {
						if (this->eventHandler != 0) {
							this->eventHandler(eventId, self, a3, this->pUserData);
						}
						ret = SCE_OK;
					}
					else {
						ret = SCE_PAF_ERROR_UI_WIDGET_EVENT_CALLBACK_UNHANDLED;
					}

					return ret;
				};

				virtual EventHandler f2()
				{
					return this->eventHandler;
				};

				SceInt32 state;
				ScePVoid pUserData;
				EventHandler eventHandler;

			};

			class Color
			{
			public:

				enum Type
				{
					Color_Text = 2,
					Color_TextShadow,
					Color_Background,
					Color_Unk5,
					Color_Unk6
				};

				Color() : r(0.0), g(0.0), b(0.0), a(0.0)
				{

				};

				~Color() { };

				SceFloat32 r;
				SceFloat32 g;
				SceFloat32 b;
				SceFloat32 a;
			};

			Widget(Widget *parent, SceInt32 a2);

			virtual ~Widget();

			//109 virtual functions here
			virtual int unkFun_008(SceInt32);
			virtual int unkFun_00C(SceInt32);
			virtual int unkFun_010(SceInt32);
			virtual int unkFun_014(SceInt32);
			virtual int unkFun_018(SceInt32);
			virtual int unkFun_01C(SceInt32);
			virtual int unkFun_020(SceInt32);
			virtual int unkFun_024(SceInt32);
			virtual int unkFun_028(SceInt32);
			virtual int unkFun_02C(SceInt32);
			virtual int unkFun_030();
			virtual int unkFun_034();
			virtual int unkFun_038();
			virtual int unkFun_03C();
			virtual int unkFun_040(SceInt32);
			virtual int unkFun_044(SceInt32);
			virtual int unkFun_048(SceInt32, SceInt32);
			virtual SceVoid SetMarkerEnable(SceBool enable);
			virtual int unkFun_050(SceInt32);
			virtual int unkFun_054();
			virtual SceVoid SetTouchEnable(SceBool enable);
			virtual SceBool GetTouchEnable();
			virtual int unkFun_060();
			virtual SceBool ContainsType(const char *pTypeName);
			virtual char *GetType();
			virtual int unkFun_06C();
			virtual int unkFun_070();
			virtual int unkFun_074(SceInt32, SceInt32);
			virtual int unkFun_078();
			virtual int unkFun_07C();
			virtual int unkFun_080();
			virtual int unkFun_084();
			virtual int unkFun_088();
			virtual int unkFun_08C();
			virtual int unkFun_090_anim(SceInt32 animationCode, SceBool setSomeFloat);
			virtual int unkFun_094_animRev(SceInt32 animationCode, SceBool setSomeFloat);
			virtual int unkFun_098();
			virtual int unkFun_09C();
			virtual int unkFun_0A0();
			virtual int unkFun_0A4();
			virtual int unkFun_0A8();
			virtual int unkFun_0AC();
			virtual int unkFun_0B0();
			virtual int unkFun_0B4();
			virtual int unkFun_0B8();
			virtual int unkFun_0BC();
			virtual int unkFun_0C0();
			virtual int unkFun_0C4();
			virtual int unkFun_0C8();
			virtual int unkFun_0CC();
			virtual int unkFun_0D0();
			virtual int unkFun_0D4();
			virtual int unkFun_0D8();
			virtual int unkFun_0DC();
			virtual int unkFun_0E0();
			virtual int unkFun_0E4();
			virtual int unkFun_0E8();
			virtual int unkFun_0EC();
			virtual int unkFun_0F0();
			virtual int unkFun_0F4();
			virtual int unkFun_0F8(SceInt32);
			virtual int unkFun_0FC();
			virtual int SetTexture(paf::graphics::Texture *tex);
			virtual int unkFun_104();
			virtual int unkFun_108();
			virtual int unkFun_10C();
			virtual int unkFun_110();
			virtual int unkFun_114();
			virtual SceInt32 SetLabelWithFlag(paf::WString *text, SceInt32 flag);
			virtual SceInt32 SetLabel(paf::WString *text);
			virtual int unkFun_120();
			virtual int unkFun_124();
			virtual int unkFun_128();
			virtual int unkFun_12C();
			virtual int unkFun_130();
			virtual int unkFun_134(SceInt32);
			virtual int unkFun_138();
			virtual int unkFun_13C();
			virtual int unkFun_140();
			virtual int unkFun_144();
			virtual int unkFun_148();
			virtual int unkFun_14C();
			virtual int unkFun_150();
			virtual int unkFun_154();
			virtual int unkFun_158();
			virtual int unkFun_15C();
			virtual int unkFun_160();
			virtual int unkFun_164();
			virtual int unkFun_168();
			virtual int unkFun_16C();
			virtual int SendEvent(SceInt32 eventId, SceBool a2);
			virtual int unkFun_174();
			virtual int unkFun_178();
			virtual int unkFun_17C();
			virtual int unkFun_180();
			virtual int unkFun_184();
			virtual int unkFun_188();
			virtual int unkFun_18C();
			virtual int unkFun_190();
			virtual int unkFun_194();
			virtual int unkFun_198();
			virtual int unkFun_19C();
			virtual int unkFun_1A0();
			virtual int unkFun_1A4();
			virtual int unkFun_1A8();

			static char *TypeText();

			//ScePafWidget_90C77CC5
			//static char *TypeSupplier(); //constructors not exported

			static char *TypeTextBox();

			static char *TypeBusyIndicator();

			//ScePafWidget_8B0C4F14
			//static char *TypeStateButtonBase(); //constructors not exported

			//ScePafWidget_F1DBC5BB
			//static char *TypeStyleStateButtonBase(); //constructors not exported

			//ScePafWidget_317FAD4E
			//static char *TypeSoundStateButtonBase(); //constructors not exported

			static char *TypePlane();

			static char *TypeBox();

			static char *TypeAppIcon2D();

			static char *TypeButton();

			static char *TypeCornerButton();

			static char *TypeDialog();

			static char *TypeFileList();

			static char *TypeImageButton();

			static char *TypeImageSlidebar();

			static char *TypeCompositeDrawer();

			static char *TypeSlidingDrawer();

			static char *TypeScrollViewParent();

			static char *TypeScrollBar2D();

			static char *TypeScrollBarTouch();

			static char *TypeScrollBar();

			static char *TypeSlideBar();

			static char *TypeRichText();

			static char *TypePageTurn();

			static char *TypeProgressBar();

			static char *TypeProgressBarTouch();

			static char *TypeRadioBox();

			static char *TypeRadioButton();

			static char *TypeListItem();

			static char *TypeListView();

			static char *TypeNumSpin();

			static char *TypeItemSpin();

			static char *TypePlaneButton();

			static char *TypeCheckBox();

			static char *TypeCompositeButton();

			static char *TypeAppIconBase();

			static char *TypeSpinBase();


			SceInt32 SetColor(Color::Type type, SceInt32 a2, SceInt32 a3, const Color *pColor);

			SceInt32 GetColor(Color::Type type, SceInt32 a2, Color *pColor);

			SceInt32 SetFilterColor(const Color *pColor, SceFloat32 a2 = 0.0f, SceInt32 a3 = 0, SceInt32 a4 = 0x10001, SceInt32 a5 = 0, SceInt32 a6 = 0, SceInt32 a7 = 0);

			enum Option
			{
				Text_Bold = 0x7,
				Text_Shadow = 0xC,
				Text_ExternalLine = 0xD
			};

			SceInt32 SetOption(Option option, SceInt32 a2, SceInt32 a3, SceBool enable);

			SceInt32 SetFontSize(SceFloat32 size, SceInt32 a2, SceSize pos, SceSize len);

			Widget *GetChildByHash(paf::Resource::Element *widgetInfo, SceUInt32 param); //param can be 0,1,2

			Widget *GetChildByNum(SceUInt32 childNum);

			Widget *GetParent();

			SceInt32 SetPosition(const SceFVector4 *pPosition, SceFloat32 a2 = 0.0f, SceInt32 a3 = 0, SceInt32 a4 = 0x10000, SceInt32 a5 = 0, SceInt32 a6 = 0, SceInt32 a7 = 0);

			SceInt32 SetSize(const SceFVector4 *pSize, SceFloat32 a2 = 0.0f, SceInt32 a3 = 0, SceInt32 a4 = 0x10004, SceInt32 a5 = 0, SceInt32 a6 = 0, SceInt32 a7 = 0);

			SceInt32 SetZoom(const SceFVector4 *pZoom, SceFloat32 a2 = 0.0f, SceInt32 a3 = 0, SceInt32 a4 = 0x10005, SceInt32 a5 = 0, SceInt32 a6 = 0, SceInt32 a7 = 0);

			SceInt32 SetAdjust(SceBool x, SceBool y, SceBool z);

			SceInt32 RegisterEventCallback(SceInt32 eventId, EventCallback *cb, SceBool a3);

			SceInt32 UnregisterEventCallback(SceInt32 eventId, SceInt32 a2, SceInt32 a3);

			SceInt32 RegisterLoopEventCallback(SceInt32 eventId, EventCallback *cb);

			SceInt32 UnregisterLoopEventCallback(SceInt32 eventId);

			SceInt32 AssignButton(SceUInt32 buttons);

			SceInt32 SetDimFactor(SceFloat32 factor, SceInt32 a2 = 0, SceInt32 a3 = 0x10003, SceInt32 a4 = 0, SceInt32 a5 = 0, SceInt32 a6 = 0);

			SceVoid Disable(SceBool a1);

			SceVoid Enable(SceBool a1);

			SceVoid SetLimitedFpsMode(SceBool enable);

			enum Effect
			{
				Effect_None = 0,
				Effect_1,
				Effect_2,
				Effect_3,
				Effect_4,
				Effect_5,
				Effect_6,
				Effect_7,
				Effect_Paceholder = 0xff,
			};

			SceInt32 SetEffect(Effect effId, SceInt32 a2);

			enum Animation
			{
				Animation_Reset = 0xFF,
				Animation_Fadein1 = 0,
				Animation_Popup1 = 1,
				Animation_Popup2 = 2,
				Animation_SlideFromBottom1 = 3,
				Animation_SlideFromRight1 = 4,
				Animation_Fadein2 = 5,
				Animation_SlideFromTop1 = 6,
				Animation_SlideFromTop2 = 7,
				Animation_SlideFromBottom2 = 8,
				Animation_SlideFromBottom3 = 9,
				Animation_Popup3 = 0xA,
				Animation_Popup4 = 0xB,
				Animation_Popup5 = 0xD,
				Animation_SlideFromTop3 = 0xE,
				Animation_SlideFromBottom4 = 0xF,
				Animation_SlideFromLeft1 = 0x10,
				Animation_SlideFromRight2 = 0x11,
				Animation_SlideFromTop4 = 0x12,
				Animation_SlideFromBottom5 = 0x13,
				Animation_SlideFromLeft2 = 0x14,
				Animation_SlideFromRight3 = 0x15,
				Animation_3D_SlideFromFront = 0x16,
				Animation_3D_SlideToBack1 = 0x17,
				A18 = 0x18,
				Animation_3D_SlideToBack2 = 0x19

			};

			SceInt32 PlayAnimation(Animation animId, EventCallback::EventHandler animCB = 0, ScePVoid pUserData = NULL);

			SceInt32 PlayAnimationReverse(Animation animId, EventCallback::EventHandler animCB = 0, ScePVoid pUserData = NULL);

			SceUChar8 unk_004[0x148];

			SceUInt32 hash;

			SceUChar8 unk_150[0x24];

			SceUInt32 childNum;

			SceUChar8 unk_178[0x1E];

			SceUInt8 animationStatus;

			SceUChar8 unk_197[0xF5];

			Color *pDisabledColor;

			SceUChar8 unk_290[0x5];

		private:

		};

		class BusyIndicator : public Widget
		{
		public:

			BusyIndicator(Widget *parent, SceInt32 a2);

			virtual ~BusyIndicator();

			SceVoid Start();

			SceVoid Stop();

			SceVoid SetBallSize(SceFloat32 size);

		private:

			SceUChar8 unk_298[0x10];
		};

		class Text : public Widget //0x2d4
		{
		public:

			Text(Widget *parent, SceInt32 a2);

			virtual ~Text();

		private:

			SceUChar8 unk_298[0x40];
		};

		class TextBox : public Widget
		{
		public:

			TextBox(Widget *parent, SceInt32 a2);

			virtual ~TextBox();
		};

		class Plane : public Widget
		{
		public:

			Plane(Widget *parent, SceInt32 a2);

			virtual ~Plane();
		};

		class SpeechBalloon : public Widget
		{
		public:

			SpeechBalloon(Widget *parent, SceInt32 a2);

			virtual ~SpeechBalloon() { }; //destructors not exported
		};

		class Box : public Widget
		{
		public:

			Box(Widget *parent, SceInt32 a2);

			virtual ~Box();

		private:

			SceUChar8 unk_298[0x568];
		};

		class AppIcon2D : public Widget
		{
		public:

			AppIcon2D(Widget *parent, SceInt32 a2);

			virtual ~AppIcon2D();
		};

		class Button : public Widget
		{
		public:

			Button(Widget *parent, SceInt32 a2);

			virtual ~Button();

			SceVoid SetThreshold(SceInt32 threshold, SceInt32 repeatThreshold);

		private:

			SceUChar8 unk_295[0x28];
		};

		class CornerButton : public Widget
		{
		public:

			CornerButton(Widget *parent, SceInt32 a2);

			virtual ~CornerButton();
		};

		class Dialog : public Widget
		{
		public:

			Dialog(Widget *parent, SceInt32 a2);

			virtual ~Dialog();
		};

		class ImageButton : public Widget
		{
		public:

			ImageButton(Widget *parent, SceInt32 a2);

			virtual ~ImageButton();
		};

		class ImageSlidebar : public Widget
		{
		public:

			ImageSlidebar(Widget *parent, SceInt32 a2);

			virtual ~ImageSlidebar();
		};

		class CompositeDrawer : public Widget
		{
		public:

			CompositeDrawer(Widget *parent, SceInt32 a2);

			virtual ~CompositeDrawer();
		};

		class SlidingDrawer : public Widget
		{
		public:

			SlidingDrawer(Widget *parent, SceInt32 a2);

			virtual ~SlidingDrawer();
		};

		class ScrollViewParent : public Widget
		{
		public:

			ScrollViewParent(Widget *parent, SceInt32 a2);

			virtual ~ScrollViewParent();
		};

		class ScrollBar2D : public Widget
		{
		public:

			ScrollBar2D(Widget *parent, SceInt32 a2);

			virtual ~ScrollBar2D();
		};

		class ScrollBarTouch : public Widget
		{
		public:

			ScrollBarTouch(Widget *parent, SceInt32 a2);

			virtual ~ScrollBarTouch();
		};

		class ScrollBar : public Widget
		{
		public:

			ScrollBar(Widget *parent, SceInt32 a2);

			virtual ~ScrollBar();
		};

		class SlideBar : public Widget
		{
		public:

			SlideBar(Widget *parent, SceInt32 a2);

			virtual ~SlideBar();
		};

		class RichText : public Widget
		{
		public:

			RichText(Widget *parent, SceInt32 a2);

			virtual ~RichText();
		};

		class PageTurn : public Widget
		{
		public:

			PageTurn(Widget *parent, SceInt32 a2);

			virtual ~PageTurn();
		};

		class ProgressBar : public Widget
		{
		public:

			ProgressBar(Widget *parent, SceInt32 a2);

			virtual ~ProgressBar();
		};

		class ProgressBarTouch : public Widget
		{
		public:

			ProgressBarTouch(Widget *parent, SceInt32 a2);

			virtual ~ProgressBarTouch();
		};

		class ListItem : public Widget //0x2d0
		{
		public:

			ListItem(Widget *parent, SceInt32 a2);

			virtual ~ListItem();

		private:

			SceUChar8 unk_2C0[0x10];
		};

		class ListView : public Widget
		{
		public:

			ListView(Widget *parent, SceInt32 a2);

			virtual ~ListView();
		};

		class FileList : public Widget
		{
		public:

			FileList(Widget *parent, SceInt32 a2);

			virtual ~FileList();
		};

		class RadioBox : public Widget
		{
		public:

			RadioBox(Widget *parent, SceInt32 a2);

			virtual ~RadioBox(); //destructors not exported
		};

		class RadioButton : public Widget
		{
		public:

			RadioButton(Widget *parent, SceInt32 a2);

			virtual ~RadioButton(); //destructors not exported
		};

		class ItemSpin : public Widget
		{
		public:

			ItemSpin(Widget *parent, SceInt32 a2);

			virtual ~ItemSpin() { }; //destructors not exported
		};

		class NumSpin : public Widget
		{
		public:

			NumSpin(Widget *parent, SceInt32 a2);

			virtual ~NumSpin() { }; //destructors not exported
		};

		class PlaneButton : public Widget
		{
		public:

			PlaneButton(Widget *parent, SceInt32 a2);

			virtual ~PlaneButton() { }; //destructors not exported
		};

		class CheckBox : public Widget
		{
		public:

			virtual int unkFun_1AC();
			virtual int unkFun_1B0();
			virtual int unkFun_1B4();
			virtual int unkFun_1B8(SceInt32);
			virtual SceVoid SetChecked(SceFloat32 delay, SceBool checked, SceInt32 a3);
			virtual SceVoid SwitchChecked();
			virtual int unkFun_1C4();
			virtual int unkFun_1C8();

			CheckBox(Widget *parent, SceInt32 a2);

			virtual ~CheckBox() { }; //destructors not exported

			SceUChar8 unk_295[0x18];

			SceUInt8 checked;
		};

		class CompositeButton : public Widget
		{
		public:

			CompositeButton(Widget *parent, SceInt32 a2);

			virtual ~CompositeButton() { }; //destructors not exported
		};

		class SpinBase : public Widget
		{
		public:

			SpinBase(Widget *parent, SceInt32 a2);

			virtual ~SpinBase() { }; //destructors not exported
		};

		class AppIconBase : public Widget
		{
		public:

			AppIconBase(Widget *parent, SceInt32 a2);

			virtual ~AppIconBase() { }; //destructors not exported
		};
	}
}