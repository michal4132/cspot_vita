#pragma once

#include <paf/stdc.h>


namespace paf {

	class String;
	class WString;

	namespace widget {
		class Widget;
	}

	namespace common {

		enum CesFlags
		{
			AllowIllegalCode	= 0x2,
			AllowOutOfCodeRange	= 0x4,
			AllowInvalidEncode	= 0x8,
			AllowSrcBufferEnd	= 0x10
		};

		static paf::widget::Widget *WidgetStateTransition(SceFloat32 delay, paf::widget::Widget *widget, SceUInt32 animationId, SceBool disableOnEnd, SceBool skipAnimation);
		static paf::widget::Widget *WidgetStateTransitionReverse(SceFloat32 delay, paf::widget::Widget *widget, SceUInt32 animationId, SceBool disableOnEnd, SceBool skipAnimation);
	}


	class String
	{
	public:

		String(const char *str);

		String(const char *str, SceSize strLength);

		String(String *src);

		String();

		~String()
		{
			if (length != 0 && *data != 0)
				delete data;
		}

		static SceSSize ToWString(const char *src, paf::WString *dst);

		static SceSSize ToWString(const char *src, paf::WString *dst, SceUInt32 flags);

		static String *WCharToNewString(const SceWChar16 *src, String *dst);

		SceSSize ToWString(paf::WString *dst);

		SceSSize ToWString(paf::WString *dst, SceUInt32 flags);

		String *Set(const char *s, SceSize srcLength)
		{
			if (data == s)
				return this;

			if (*data != 0)
				delete data;

			if (srcLength == 0 || s == NULL || s[0] == 0) {
				String();
				return this;
			}

			data = new char[srcLength + 1];
			length = srcLength;
			sce_paf_memcpy(data, s, length);
			data[length] = 0;

			return this;
		};

		String *Set(const char *s)
		{
			Set(s, sce_paf_strlen(s));
			return this;
		};

		String *operator=(const String *s)
		{
			if (data == s->data)
				return this;

			if (s->length == 0) {
				String();
				return this;
			}

			data = new char[s->length + 1];
			length = s->length;
			sce_paf_memcpy(data, s->data, s->length + 1);

			return this;
		}

		String *operator+(const String *s) const
		{
			String *newStr = new String;

			if (!length && !s->length)
			{
				return newStr;
			}

			newStr->data = new char[s->length + length + 1];
			newStr->length = s->length + length;
			sce_paf_memcpy(newStr->data, data, length);
			sce_paf_memcpy(&newStr->data[length], s->data, s->length + 1);
			newStr->data[newStr->length] = 0;
			return newStr;
		}

		SceBool operator==(const String *s) const
		{
			if (s->length != length)
				return SCE_FALSE;

			if (!sce_paf_strncmp(s->data, data, length))
				return SCE_TRUE;
			else
				return SCE_FALSE;
		}

		SceVoid Clear()
		{
			if (length != 0 && data != NULL) {
				delete data;
				String();
			}
		}

		char *data;
		SceSize length;

	private:

		SceUInt32 m_work;

	};

	class WString
	{
	public:

		WString() : data(NULL), length(0)
		{

		}

		WString(const SceWChar16 *s, SceSize srcLength) : data(NULL), length(0)
		{
			if (srcLength == 0 || s == NULL || s[0] == 0) {
				return;
			}

			data = new SceWChar16[srcLength + 1];
			length = srcLength;
			sce_paf_wmemcpy((wchar_t*)data, (wchar_t*)s, length);
			data[length] = 0;

			return;
		}

		WString(const SceWChar16 *s) : data(NULL), length(0)
		{
			SceSize srcLength = sce_paf_wcslen((wchar_t*)s);

			if (srcLength == 0 || s == NULL || s[0] == 0) {
				return;
			}

			data = new SceWChar16[srcLength + 1];
			length = srcLength;
			sce_paf_wmemcpy((wchar_t*)data, (wchar_t*)s, length);
			data[length] = 0;

			return;
		}

		~WString()
		{
			if (length != 0 && data != NULL)
				delete data;
		}

		static SceSSize ToString(const SceWChar16 *src, String *dst);

		static SceSSize ToString(const SceWChar16 *src, String *dst, SceUInt32 flags);

		static WString *CharToNewWString(const char *src, WString *dst);

		SceSSize ToString(String *dst);

		SceSSize ToString(String *dst, SceUInt32 flags);

		WString *Set(const SceWChar16 *s, SceSize srcLength)
		{
			if (data == s)
				return this;

			if (data != NULL)
				delete data;

			if (srcLength == 0 || s == NULL || s[0] == 0) {
				data = NULL;
				length = 0;
				return this;
			}

			data = new SceWChar16[srcLength + 1];
			length = srcLength;
			sce_paf_wmemcpy((wchar_t*)data, (wchar_t*)s, length);
			data[length] = 0;

			return this;
		};

		WString *Set(const SceWChar16 *s)
		{
			Set(s, sce_paf_wcslen((wchar_t *)s));
			return this;
		};

		WString *operator=(const WString *s)
		{
			if (data == s->data)
				return this;

			if (s->length == 0) {
				data = NULL;
				length = 0;
				return this;
			}

			data = new SceWChar16[s->length + 1];
			length = s->length;
			sce_paf_wmemcpy((wchar_t*)data, (wchar_t*)s->data, s->length + 1);

			return this;
		}

		WString *operator+(const String *s) const
		{
			WString *newStr = new WString;

			if (!length && !s->length)
			{
				return newStr;
			}

			newStr->data = new SceWChar16[s->length + length + 1];
			newStr->length = s->length + length;
			sce_paf_wmemcpy((wchar_t*)newStr->data, (wchar_t*)data, length);
			sce_paf_wmemcpy((wchar_t*)&newStr->data[length], (wchar_t*)s->data, s->length + 1);
			newStr->data[newStr->length] = 0;
			return newStr;
		}

		SceBool operator==(const WString *s) const
		{
			if (s->length != length)
				return SCE_FALSE;

			if (!sce_paf_wcsncmp((wchar_t*)s->data, (wchar_t*)data, length))
				return SCE_TRUE;
			else
				return SCE_FALSE;
		}

		SceVoid Clear()
		{
			if (length != 0 && data != NULL) {
				delete data;
				length = 0;
				data = NULL;
			}
		}

		SceWChar16 *data;
		SceSize length;

	private:

		SceUInt32 m_work;
	};

	namespace common {

		class Utils
		{
		public:

			typedef void(*MainThreadTaskEntryFunction)(void *pArgBlock);

			Utils();

			~Utils();

			static SceVoid AddMainThreadTask(MainThreadTaskEntryFunction entry, ScePVoid pArgBlock);

			static SceVoid RemoveMainThreadTask(MainThreadTaskEntryFunction entry, ScePVoid pArgBlock);
		};

	}
}