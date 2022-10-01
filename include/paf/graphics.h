#pragma once

#include <psp2/kernel/sysmem.h> 
#include <psp2common/kernel/sysmem.h> 
#include <psp2/gxm.h>
#include <psp2/types.h>
#include <paf/misc.h>


namespace paf {

	namespace graphics {

		class MemoryPool
		{
		public:

			class InitParam
			{
			public:

				ScePVoid pMemory;
				SceSize size;
				char *pName;
				paf::Allocator::Opt *pAllocatorOpt;
				ScePVoid pUnkAllocatorOpt;
				SceUInt32 memoryType;
			};

			MemoryPool(InitParam *pInitParam);

			MemoryPool(ScePVoid pMemory, SceSize size, const char *pName, paf::Allocator::Opt *pAllocatorOpt = NULL, SceUInt32 memoryType = 0);

			virtual ~MemoryPool();

			virtual int unkf1();
			virtual int unkf2();
			virtual int unkf3();

			enum MemoryType
			{
				MemoryType_UserNC,
				MemoryType_CDRAM,
				MemoryType_Shared,
			};

			enum MemoryTypeSpecial
			{
				MemoryTypeSpecial_Unused,
				MemoryTypeSpecial_Unk0xCF08060,
				MemoryTypeSpecial_Phycont
			};

			enum GpuMapType
			{
				GpuMapType_Normal,
				GpuMapType_VertexUsse,
				GpuMapType_FragmentUsse,
				GpuMapType_NoMap,
			};

			class MemBlockOptParam
			{
			public:

				MemoryTypeSpecial memoryTypeSpecial;
				SceKernelAllocMemBlockAttr *memBlockOpt;
				GpuMapType gpuMapType;
				SceUInt32 mapAttribs;
				SceUInt32 usseOffset;
				SceUInt32 unused;
			};

			static ScePVoid AllocMemBlock(MemoryType memoryType, SceSize size, const char *pName, ScePVoid pOptParam = NULL);

			static SceVoid FreeMemBlock(MemoryType memoryType, ScePVoid pMemory);

			static MemoryPool *GetDefaultMemoryPool();

		private:

			SceUChar8 m_work[0x48];

		};

		class Surface
		{
		public:

			Surface(MemoryPool *pMemPool, SceUInt32 width, SceUInt32 height, SceUInt32 format, SceUInt32 orderType, SceInt32 a7, SceUInt32 numLevel, SceInt32 a9);

			virtual ~Surface();

			virtual int unkf1();
			virtual int unkf2();
			virtual int unkf3();
			virtual int unkf4();
			virtual int unkf5();
			virtual int unkf6();
			virtual int unkf7();
			virtual int unkf8();
			virtual int unkf9();
			virtual int unkf10();

			void* operator new(size_t size);

			void operator delete(void*);

		private:

			SceUChar8 m_work[0x44];

		};

		class Texture
		{
		public:

			Texture() { };

			~Texture() { };

			Surface *texSurface;

		private:

			SceUChar8 unk[0x4];

		};

	}
}