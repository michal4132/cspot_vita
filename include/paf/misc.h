#pragma once

#include <psp2kern/kernel/utils.h> 
#include <psp2common/kernel/iofilemgr.h> 
#include <psp2/rtc.h>
#include <paf/common.h>

// TODO
#define SCE_UID_INVALID_UID -1

typedef struct SceIoAsyncParam {
  int result; // [out] result of the IO operation (e.g. UID, read/wrote size, error code)
  int unk_04; // [out]
  int unk_08; // [in]
  int unk_0C; // [out]
  int unk_10; // [out]
  int unk_14; // [out]
} SceIoAsyncParam;


namespace paf {

	static SceBool IsDolce();

	namespace datetime {

		class DateTime
		{
		public:

			DateTime
			(
				SceUInt16 year,
				SceUInt16 month,
				SceUInt16 day,
				SceUInt16 hour,
				SceUInt16 minute,
				SceUInt16 second,
				SceUInt32 microsecond
			);

			DateTime() { };

			~DateTime();

			static SceInt32 IsLeapYear(SceInt32 year);

			static SceInt32 GetDaysInMonth(SceInt32 year, SceInt32 month);

			static SceInt32 GetCurrentTick(SceRtcTick *tick);

			static SceInt32 ConvertUtcToLoacalTime(const SceRtcTick *utc, SceRtcTick *localTime);

			static SceInt32 GetCurrentTickLocalTime(SceRtcTick *tick);

			static SceInt32 GetCurrentNetworkTick(SceRtcTick *tick);

			static SceInt32 ConvertLocalTimeToUtc(const SceRtcTick *localTime, SceRtcTick *utc);

			static SceInt32 FormatRFC3339(char *pszDateTime, SceRtcTick *tick, SceInt32 iTimeZoneMinutes);

			static SceInt32 FormatRFC3339LocalTime(char *pszDateTime, SceRtcTick *tick);

			static SceInt32 ParseRFC3339(SceRtcTick *tick, const char *pszDateTime);

			SceVoid SetFromTick(SceRtcTick *tick);

			SceVoid SetFromRFC3339(const char *pszDateTime);

			SceVoid GetCurrentClockUtc();

			SceVoid GetCurrentClockLocalTime();

			SceVoid GetCurrentNetworkClock();

			SceRtcTick ToRtcTick();

			SceVoid ToRtcTick(SceRtcTick *tick);

			SceVoid Sum(const DateTime *s1, const DateTime *s2);

			SceBool operator<(const DateTime *pDt);

			SceUInt16 year;
			SceUInt16 month;
			SceUInt16 day;
			SceUInt16 hour;
			SceUInt16 minute;
			SceUInt16 second;
			SceUInt32 microsecond;
			SceUInt32 unk_10;
			SceUInt8 unk_14;

		};

	}

	namespace io {

		enum Error
		{
			SCE_PAF_ERROR_MISC_IO_INVALID_POINTER = 0x80AF5901
		};

		class File
		{
		public:

			class AsyncResult
			{
			public:

				SceInt32 result;
				SceInt32 unk_04;
			};

			File();

			virtual ~File();

			virtual SceInt32 Open(const char *filename, int flag, SceIoMode mode);
			virtual SceInt32 Close();
			virtual SceInt32 Read(void *buf, SceSize nbyte);
			virtual SceInt32 Write(const void *buf, SceSize nbyte);
			virtual SceInt32 Lseek(SceOff offset, int whence);
			virtual SceInt32 Sync();
			virtual SceInt32 OpenAsync(const char *filename, int flags, SceMode mode);
			virtual SceInt32 CloseAsync();
			virtual SceInt32 ReadAsync(void *buf, SceSize nbyte);
			virtual SceInt32 WriteAsync(const void *buf, SceSize nbyte);
			virtual SceInt32 LseekAsync(SceInt32 a1, int whence, SceIoAsyncParam* asyncParam);
			virtual SceInt32 SyncAsync();
			virtual SceInt32 WaitAsync(AsyncResult *pResult);
			virtual SceInt32 PollAsync(AsyncResult *pResult);
			virtual SceBool IsOpened();
			virtual SceInt32 unkFun_44();
			virtual SceInt32 CancelAsync();
			virtual SceOff GetSize();
			virtual SceInt32 GetStat(ScePVoid pafStat);
			virtual SceInt32 SetPriority(SceUInt32 ioPriority);

		private:

			SceUChar8 m_work[0x2C];

		};

		class Dir
		{
		public:

			class Dirent
			{
			public:

				SceInt32 type;
				paf::String name;
				SceInt32 size;
				SceUInt32 creationYear;
			};

			Dir() : uid(SCE_UID_INVALID_UID) { };

			~Dir() { };

			SceInt32 Open(const char *dirname);

			SceInt32 Close();

			SceInt32 Read(Dirent *stat);

			SceUID uid;

		};

		static SceInt32 SetDefaultPriority(SceUInt32 ioPriority);

		static SceBool Exists(const char *path);

		static SceBool IsDir(const char *path);

		static SceInt32 Remove(const char *path);

		static SceInt32 RemoveRecursive(const char *path);

		static SceInt32 Mkdir(const char *dirname, SceIoMode mode);

		static SceInt32 MkdirRWSYS(const char *dirname);

		//dot-separated path list
		static SceInt32 MkdirMulti(const char *dirnameList, SceIoMode mode);

		static SceInt32 MkdirMultiRWSYS(const char *dirnameList);
	}

	class Module
	{
	public:

		Module(const char *pPath, SceInt32 unused_a2, SceUInt32 flags, SceInt32 unused_a4);

		~Module();

	private:

		SceUChar8 m_work[0x4];

	};

	class Sha1
	{
	public:

		Sha1();

		~Sha1() { };

		SceInt32 BlockUpdate(const ScePVoid plain, SceUInt32 len);

		SceInt32 BlockResult(SceUChar8 *digest);

	private:

		SceSha1Context m_sha;

	};

	class Allocator
	{
	public:

		class Opt
		{
		public:

			Opt() { };

			~Opt() { };

			int a1;
			int a2;
			SceUInt8 a3_0;
			SceUInt8 a3_1;
			SceUInt8 a3_2;
			SceUInt8 a3_3;
			SceBool useCdlgMemory;
			SceSize alignment;
		};

		Allocator(ScePVoid *membase, SceSize size, const char *name, Opt *pOpt = NULL);

		virtual ~Allocator();

		static Allocator *GetGlobalAllocator();

		SceSize GetSize();

		SceVoid GetMemBlockBase(ScePVoid *membase, ScePVoid *membaseTop);

		SceVoid Deallocate(ScePVoid ptr);

		ScePVoid Allocate(SceSize size);

		ScePVoid AllocateAligned(SceSize alignment, SceSize size);

	private:

		SceUChar8 m_work[0x5C];

	};

	class Image
	{
	public:

		Image();

		~Image();

		enum Type
		{
			Type_0,
			Type_PNG,
			Type_JPEG,
			Type_TIFF,
			Type_GIF,
			Type_BMP,
			Type_GIM,
			Type_DDS
		};


		class Result
		{
		public:

			Image *img;
			ScePVoid unk_04;
		};

		static SceVoid Load(Result *pRes, const char *pPath, ScePVoid buf, SceInt32 a4, SceInt32 a5, SceInt32 a6, SceInt32 a7);

		static SceInt32 GetLastError();

		static SceBool SetLastError(SceInt32 errorCode);

		SceInt32 GetType();

		SceInt32 GetPaletteColorNum();
	};
}