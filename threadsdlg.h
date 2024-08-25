#ifndef THREADSDLG_H
#define THREADSDLG_H

#include "switcher.h"
#include <QDialog>
#include <QMutex>

using ulong64 = unsigned long long int;

QT_BEGIN_NAMESPACE
namespace Ui { class ThreadsDlg; }
QT_END_NAMESPACE

class ThreadsDlg : public QDialog
{
    Q_OBJECT
public:
    ThreadsDlg(QWidget *parent = nullptr);
    ~ThreadsDlg();
protected:
    void closeEvent(QCloseEvent* e) override;
private:
    Ui::ThreadsDlg *ui;
    ulong64 m_nCounter = 0L; // Global counter
    QMutex m_mutex;          // Main mutex
    VECT_SWITCH m_vect_swt;  // Collection of switchers
    void CreateThreads(int iTotalThreads);
private slots:
    void OnClose();
    void OnStart();
    void slotGlobalIncrement(int n_switcher_id, int n_count);  
};
#endif // THREADSDLG_H
