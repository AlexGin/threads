#pragma once

#include <QObject>
#include <vector>
#include <memory>

constexpr int MAX_COUNT = 100;

class Switcher : public QObject
{
   Q_OBJECT
   public:
       Switcher(int id, QObject* pTarget);
       ~Switcher();
       void process();
   private:
       QObject* m_pTarget = nullptr;
       int m_iPassCounter = 0;
       int m_id = (-1); // Inactive value (switcher is not existing)
   signals:
       void signalGlobalIncrement(int n_switcher_id, int n_counter);
       void signalWorkFinished();
};
using SP_SWITCH = std::shared_ptr<Switcher>;
using VECT_SWITCH = std::vector<std::shared_ptr<Switcher>>;
