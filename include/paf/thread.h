#pragma once

#include <psp2/kernel/threadmgr.h> 


namespace paf {
	namespace thread {

		class DefaultThreadPriority
		{
		public:

			DefaultThreadPriority();

			static DefaultThreadPriority *Get();

			SceVoid Set();

			SceInt32 priority1;
			SceInt32 priority2;
			SceInt32 priority3;

		};

		class Thread
		{
		public:

			enum Error
			{
				SCE_PAF_ERROR_THREAD_THREAD_ALREADY_STARTED = 0x80AF0702
			};

			class Opt
			{
			public:

				Opt();

				~Opt() { };

				SceUInt32 attr;
				SceInt32 cpuAffinityMask;
				SceKernelMemBlockType stackMemoryType;

			};


			Thread(SceInt32 initPriority, SceSize stackSize, const char *pName, const Opt *opt = SCE_NULL);

			// virtual ~Thread();

			virtual SceVoid EntryFunction() = 0;

			static SceInt32 InternalThreadEntry(SceSize argSize, ScePVoid pArgBlock);

			static SceVoid SetMainThreadUID(SceInt32 threadNum = 0);

			static SceUID GetMainThreadUID(SceInt32 threadNum = 0);

			static SceBool IsMainThread(SceInt32 threadNum = 0);

			static SceVoid Sleep(SceUInt32 msec);

			SceInt32 Start();

			SceVoid Join();

			SceVoid ChangePriority(SceInt32 priority);

			SceUID Self();

			SceBool IsStarted();

			SceVoid Cancel();

			//related to unk variable, unk set to SCE_FALSE by default
			//ScePafThread_F95B2895 set unk to SCE_TRUE
			//ScePafThread_4CE6D88B get unk

		private:

			SceUChar8 m_work[0x14];

		};

		class Sema
		{
		public:

			Sema(const char *pName, SceInt32 initCount, SceInt32 maxCount);

			~Sema();

			SceVoid Wait();

			SceVoid Signal();

		private:

			SceUID m_uid;

		};

		class Mutex
		{
		public:

			Mutex(const char *pName, SceBool isFifo);

			~Mutex();

			SceVoid Lock();

			SceBool TryLock();

			SceVoid Unlock();

		private:

			SceInt64 m_work[4];

		};

		class Mutex2
		{
		public:

			Mutex2(const char *pName, SceBool isFifo);

			~Mutex2();

			SceVoid Lock();

			SceBool TryLock();

			SceVoid Unlock();

		private:

			SceInt64 m_work[4];

		};

		class Cond
		{
		public:

			Cond(SceKernelLwMutexWork *pLwMutex, const char *pName, SceBool isFifo);

			~Cond();

			SceBool Wait(SceUInt32 timeoutMsec = 0);

			SceVoid Notify();

			SceVoid NotifyAll();

		private:

			SceInt32 m_work[5];

		};

		class RWLock
		{
		public:

			RWLock(const char *pName);

			~RWLock();

			SceVoid LockShared();

			SceVoid UnlockShared();

			SceVoid LockExclusive();

			SceVoid UnlockExclusive();

		private:

			SceInt32 m_work[0x8];

		};

		class Queue
		{
		public:

			typedef void(*QueueEntryFunction)(void *pArgBlock);

			Queue(const char *pName);

			~Queue();

			static Queue *s_mainThreadQueue;

			SceVoid WaitDone();

			SceVoid Push(const char *pName, QueueEntryFunction entry, ScePVoid pArgBlock);

			SceVoid Execute();

			SceUInt32 GetSize();

		private:

			SceUChar8 m_work[0x30];

		};

		class SyncCall
		{
		public:

			typedef void(*SyncCallEntryFunction)(void *pArgBlock);

			SyncCall(const char *pName);

			~SyncCall();

			static SyncCall *s_mainThreadSyncCall;

			SceVoid Push(SyncCallEntryFunction entry, ScePVoid pArgBlock);

			SceVoid Execute();

		private:

			SceUChar8 m_work[0x10];

		};

		//Not usable yet
		/*class JobQueue
		{
		public:

			enum Error
			{
				SCE_PAF_ERROR_THREAD_JOBQUEUE_ALREADY_PUSHED = 0x80AF0901,
				SCE_PAF_ERROR_THREAD_JOBQUEUE_UNK = 0x80AF0902
			};

			class Opt
			{
			public:

				Opt();

				~Opt() { };

				SceInt32 workerPriority;
				SceSize workerStackSize;
				SceUInt32 workerNum;
				Thread::Opt *workerOpt;

			};

			class Item;

			class Request
			{
			public:

				typedef void(*RequestEntryFunction)(Item *job);

				Item *job;
				SceInt32 unk_04;
				SceInt32 unk_08;
				RequestEntryFunction entry;

			};

			class Item
			{
			public:

				Item(const char *pName);

				virtual ~Item();

				virtual SceVoid SetSomething();

				virtual SceVoid EntryFunction() = 0;

				virtual SceVoid FinishCallback() = 0;

				Request *req;

			private:

				SceUChar8 m_work[0x24];

			};

			typedef void(*QueueEntryFunction)(void *pArgBlock);

			JobQueue(const char *pName, Opt *opt = SCE_NULL);

			~JobQueue();

			static JobQueue *s_defaultJobQueue;

			static SceVoid CreateDefaultJobQueue(Opt *opt);

			static SceVoid DeleteDefaultJobQueue();

			SceInt32 Push(Item *queueItem);

			SceVoid Join();

			SceUInt32 GetSize();

			SceVoid ChangeWorkerPriority(SceInt32 priority);

			SceVoid Execute();

			SceVoid Execute2();

		private:

			SceUChar8 m_work[0x2C];

		};*/
	}
}