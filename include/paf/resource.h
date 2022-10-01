#pragma once

#include <paf/common.h>

namespace paf {

	class Allocator;

	class Resource
	{
	public:

		enum Error
		{
			SCE_PAF_ERROR_RESOURCE_NOT_FOUND = 0x80AF0601
		};

		typedef void(*ResourceMemAllocator)(SceUInt32 opType, paf::Allocator *allocator, ScePVoid ptrIn, SceSize sizeIn, ScePVoid **ptrOut, SceSize *sizeOut);

		class LoadParam
		{
		public:

			SceUInt32 flags;
			paf::Allocator *memAllocator;
			ResourceMemAllocator memAllocFunction;
		};

		Resource(const char *resourcePath, LoadParam *resLoadParam);

		~Resource();

		static SceVoid DefaultMemAllocator(SceUInt32 opType, paf::Allocator *allocator, ScePVoid ptrIn, SceSize sizeIn, ScePVoid **ptrOut, SceSize *sizeOut);

		class Element
		{
		public:

			Element() : hash(0)
			{

			}

			~Element()
			{

			}

			SceUInt32 GetHashById(Element *src);

			paf::String id;
			SceUInt32 hash;
		};

		SceInt32 GetString(SceUInt32 hash, SceWChar16 **ppString, SceSize *pStringSize);

		SceInt32 GetString(const char *id, SceWChar16 **ppString, SceSize *pStringSize);
	};

}