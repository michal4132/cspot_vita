#ifndef BELL_QUEUE_H
#define BELL_QUEUE_H

#include <queue>
#include <atomic>
#include <psp2/kernel/threadmgr.h>

namespace bell
{
    template <typename dataType>
    class Queue
    {
    private:
        /// Queue
        std::queue<dataType> m_queue;
        /// Mutex to controll multiple access
        int mutexid = sceKernelCreateMutex("m_mutex", 0, 1, NULL);
        /// Conditional variable used to fire event
        int m_cv = sceKernelCreateCond("cond_var", 0, mutexid, NULL);;
        /// Atomic variable used to terminate immediately wpop and wtpop functions
        std::atomic<bool> m_forceExit = false;

    public:
        /// <summary> Add a new element in the queue. </summary>
        /// <param name="data"> New element. </param>
        void push(dataType const &data)
        {
            m_forceExit.store(false);
            sceKernelLockMutex(mutexid, 0, 0);
            m_queue.push(data);
            sceKernelUnlockMutex(mutexid, 0);
            sceKernelSignalCond(m_cv);
        }
        /// <summary> Check queue empty. </summary>
        /// <returns> True if the queue is empty. </returns>
        bool isEmpty() const
        {
            // std::unique_lock<std::mutex> lk(m_mutex);
            return m_queue.empty();
        }
        /// <summary> Pop element from queue. </summary>
        /// <param name="popped_value"> [in,out] Element. </param>
        /// <returns> false if the queue is empty. </returns>
        bool pop(dataType &popped_value)
        {
            sceKernelLockMutex(mutexid, 0, 0);
            if (m_queue.empty())
            {
                sceKernelUnlockMutex(mutexid, 0);
                return false;
            }
            else
            {
                popped_value = m_queue.front();
                m_queue.pop();
                sceKernelUnlockMutex(mutexid, 0);
                return true;
            }
        }
        /// <summary> Wait and pop an element in the queue. </summary>
        /// <param name="popped_value"> [in,out] Element. </param>
        ///  <returns> False for forced exit. </returns>
        bool wpop(dataType &popped_value)
        {
            sceKernelLockMutex(mutexid, 0, 0);
            // m_cv.wait(lk, [&]() -> bool
                    //   { return !m_queue.empty() || m_forceExit.load(); });
            uint32_t us = 0;
            sceKernelWaitCond(m_cv, &us);
            if (m_forceExit.load())
                return false;
            popped_value = m_queue.front();
            m_queue.pop();
            sceKernelUnlockMutex(mutexid, 0);
            return true;
        }
        /// <summary> Timed wait and pop an element in the queue. </summary>
        /// <param name="popped_value"> [in,out] Element. </param>
        /// <param name="milliseconds"> [in] Wait time. </param>
        ///  <returns> False for timeout or forced exit. </returns>
        bool wtpop(dataType &popped_value, long milliseconds = 1000)
        {
            sceKernelLockMutex(mutexid, 0, 0);
            // m_cv.wait_for(lk, std::chrono::milliseconds(milliseconds), [&]() -> bool
                        //   { return !m_queue.empty() || m_forceExit.load(); });
            uint32_t us = milliseconds * 1000;
            sceKernelWaitCond(m_cv, &us);
            if (m_forceExit.load())
                return false;
            if (m_queue.empty())
                return false;
            popped_value = m_queue.front();
            m_queue.pop();
            sceKernelUnlockMutex(mutexid, 0);
            return true;
        }
        /// <summary> Queue size. </summary>
        int size()
        {
            return static_cast<int>(m_queue.size());
        }
        /// <summary> Free the queue and force stop. </summary>
        void clear()
        {
            m_forceExit.store(true);
            sceKernelLockMutex(mutexid, 0, 0);
            while (!m_queue.empty())
            {
                //delete m_queue.front();
                m_queue.pop();
            }
            sceKernelUnlockMutex(mutexid, 0);
            sceKernelSignalCond(m_cv);
        }
        /// <summary> Check queue in forced exit state. </summary>
        bool isExit() const
        {
            return m_forceExit.load();
        }
    };
}

#endif