#include <QMutexLocker>
#include <QThread>
#include <QMessageBox>

#include "threadsdlg.h"
#include "ui_threadsdlg.h"

#include "switcher.h"
#include "LogFile.h"

#ifdef Q_OS_LINUX
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)
#endif

extern CLogFile g_logger;

ThreadsDlg::ThreadsDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ThreadsDlg)
{
    ui->setupUi(this);

    // remove question mark from the title bar
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QString strTitle{"THREADS - Execution's threads..."};
    this->setWindowTitle(strTitle);

    connect(ui->pushButtonExit, &QAbstractButton::clicked, this, &ThreadsDlg::OnClose);
    connect(ui->pushButtonStart, &QAbstractButton::clicked, this, &ThreadsDlg::OnStart);
}

ThreadsDlg::~ThreadsDlg()
{
    delete ui;
}

void ThreadsDlg::closeEvent(QCloseEvent* e)
{
    Q_UNUSED(e);
    g_logger.SaveLogFile(LOG_MODES::DEBUGING, "Exit: 'Threads' - Application exit");
}

void ThreadsDlg::CreateThreads(int iTotalThreads)
{
    for(int iIndex = 0; iIndex < iTotalThreads; iIndex++)
    {
        SP_SWITCH spSwitch(new Switcher(iIndex, this));
        m_vect_swt.push_back(spSwitch);

        QThread* pThread = new QThread();
        Switcher* pSwitcher = spSwitch.get();
        pSwitcher->moveToThread(pThread);
        connect(pThread, &QThread::started, pSwitcher, &Switcher::process);
        connect(pSwitcher, &Switcher::signalWorkFinished, pThread, &QThread::quit);
        pThread->start();
    }
}

void ThreadsDlg::OnClose()
{
    this->close();
}

void ThreadsDlg::OnStart()
{
    ui->pushButtonStart->setEnabled(false);

    bool bOk = false;
    QString strText = ui->lineEdit->text();
    int nTotalThreads = strText.toInt(&bOk);
    if (!bOk)
    {
        QMessageBox::warning(this, "Total Threads of Execution",
                "Not valid input data! Execution cancelled!");
        ui->pushButtonStart->setEnabled(true);
        ui->lineEdit->setText("");
        return;
    }
    g_logger.SaveLogFile(LOG_MODES::DEBUGING, "Starting: Execution started. Total Threads-of-Execution: %u.", nTotalThreads);
    CreateThreads(nTotalThreads);
}

void ThreadsDlg::slotGlobalIncrement(int n_switcher_id, int n_count)
{
    QMutexLocker ml(&m_mutex);
    m_nCounter++;
    long n_tid =
#ifdef Q_OS_LINUX
            gettid();
#else
            ::GetCurrentThreadId();
#endif
    g_logger.SaveLogFile(LOG_MODES::DEBUGING,
            "Incremrnting: Switcher-ID = %u; Count = %u; Result = %u; Thread ID = %u.",
            n_switcher_id, n_count, m_nCounter, n_tid);

    if (m_vect_swt.size() * MAX_COUNT == m_nCounter)
    {
        g_logger.SaveLogFile(LOG_MODES::DEBUGING, "RESULT IS READY: Result = %u.", m_nCounter);
        ui->lcdNumberResult->display(static_cast<double>(m_nCounter));
    }
}
