#include <QThread>

#include "switcher.h"
#include "LogFile.h"

#ifdef Q_OS_LINUX
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
#endif

extern CLogFile g_logger;

Switcher::Switcher(int id, QObject* pTarget)
    : m_pTarget(pTarget), m_id(id)
{
    long n_tid =
#ifdef Q_OS_LINUX
            gettid();
#else
            ::GetCurrentThreadId();
#endif
    g_logger.SaveLogFile(LOG_MODES::DEBUGING,
            "Creating new switcher (Switcher-ID = %u; Thread ID = %u).", m_id, n_tid);

    connect(this, SIGNAL(signalGlobalIncrement(int, int)),
            m_pTarget, SLOT(slotGlobalIncrement(int, int)), Qt::QueuedConnection);
}

Switcher::~Switcher()
{
    long n_tid =
#ifdef Q_OS_LINUX
            gettid();
#else
            ::GetCurrentThreadId();
#endif
    g_logger.SaveLogFile(LOG_MODES::DEBUGING,
            "Destroying existing switcher (Switcher-ID = %u; Thread ID = %u).", m_id, n_tid);
}

void Switcher::process()
{
    for (;;)
    {
        long n_tid =
#ifdef Q_OS_LINUX
            gettid();
#else
            ::GetCurrentThreadId();
#endif
        QThread::yieldCurrentThread();
        if (m_iPassCounter < 100)
        {
            emit signalGlobalIncrement(m_id, m_iPassCounter++);
            g_logger.SaveLogFile(LOG_MODES::DEBUGING,
                    "REQUEST to Incremrnting: Switcher-ID = %u; Count = %u; Thread ID = %u.",
                    m_id, m_iPassCounter, n_tid);
        }
        else
        {
            g_logger.SaveLogFile(LOG_MODES::DEBUGING,
                    "THREAD FINISHING: Switcher-ID = %u; Thread ID = %u", m_id, n_tid);
            emit signalWorkFinished();
            break;
        }
    }
}
