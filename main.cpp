#include "threadsdlg.h"

#include <QApplication>
#include <QIcon>

#ifdef Q_OS_LINUX
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
#endif

#include "LogFile.h"

CLogFile g_logger("Threads"); // Do not using "g_log" name!
QApplication* g_pApp;

//Get working folder (for settings, logs and other)
QString GetWorkingFolder();

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/threads.jpg"));
    g_pApp = &app;
#ifdef Q_OS_LINUX
    long n_pid = getpid();
    g_logger.SaveLogFile(LOG_MODES::DEBUGING, "Threads: main - Application starting (Process ID = %u)", n_pid);
#endif
#ifdef Q_OS_WIN
    long n_tid = ::GetCurrentThreadId();
    g_logger.SaveLogFile(LOG_MODES::DEBUGING, "Threads: main - Application starting. MainThread ID=%u.", n_tid);
#endif
    ThreadsDlg w;
    w.show();
    return app.exec();
}

//Get working folder (for settings, logs and other)
QString GetWorkingFolder()
{
    //If application start from one of the follows folders
    //Make settings file in the parent folder
    //Otherwise in the same folder
    QString strPath = g_pApp->applicationDirPath();
    if (strPath.right(5) == "Debug" ||
        strPath.right(5) == "debug" ||
        strPath.right(7) == "Release" ||
        strPath.right(7) == "release" ||
        strPath.right(3) == "bin" ||
        strPath.right(3) == "BIN" ||
        strPath.right(3) == "Bin" )
    {
        int i;
        for (i = strPath.length() - 1; i > 0 && strPath[i] != '/'; --i);
        if (i)
            strPath = strPath.left(i);
    }
    //Add slash to the end
    if (strPath.right(1) != "/")
        strPath += "/";
    return strPath;
}
