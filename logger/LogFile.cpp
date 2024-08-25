#include "LogFile.h"

#include <QMessageBox>
#include <QCoreApplication>

//////////////////////////////////////////////////////////////////////

CLogFile::CLogFile(const std::string& sApplicationName)
	: m_sApplicationName(sApplicationName)
{
}

CLogFile::CLogFile(void)
{
}

CLogFile::~CLogFile(void)
{
}

void CLogFile::SetApplicationName(const std::string& sApplicationName)
{
	m_sApplicationName = sApplicationName;
}

std::string CLogFile::GetApplicationName() const
{
	return m_sApplicationName;
}

void CLogFile::SetLogSettings(const LOG_FILE_SETTINGS& logFileSettings)
{
	m_logFileSettings = logFileSettings;
}

std::string CLogFile::GetStrLogFilePath() const
{
	return std::get<0>(m_logFileSettings);
}

bool CLogFile::GetIsLogAutoRemoveEnable() const
{
	return std::get<1>(m_logFileSettings);
}

int CLogFile::GetLogRemoveDays() const
{
	return std::get<2>(m_logFileSettings);
}

std::string CLogFile::GetCategory(LOG_MODES Category)
{
	std::string sResult;
	switch (Category)
	{
	case LOG_MODES::ERRORLOG: 	sResult = "[ERRORLOG]";	break;
	case LOG_MODES::DEBUGING:	sResult = "[DEBUGING]"; break;
	case LOG_MODES::WARNING:    sResult = "[WARNING]";  break;
	case LOG_MODES::TESTLOG:  	sResult = "[TESTLOG]";  break;
	case LOG_MODES::MATHLOG:  	sResult = "[MATHLOG]";  break;
	}
	return sResult;
}

void CLogFile::SaveLogFile(LOG_MODES Category, const char* pszFormat, ...)
{
	bool bShowMs = (bool)(Category == LOG_MODES::MATHLOG || Category == LOG_MODES::DEBUGING || Category == LOG_MODES::TESTLOG);

	m_qmLogFile.lock();

    char szLog[4096]; /* [2048]; */
	va_list argList;
	va_start(argList, pszFormat);
	vsprintf(szLog, pszFormat, argList);
	va_end(argList);

	std::string sLog(szLog);
	SaveLogCore(sLog, bShowMs, Category);
	
	m_qmLogFile.unlock();
}

std::string CLogFile::PrepareFullLogPath()
{
	QString strPath = QCoreApplication::applicationDirPath(); // QCoreApplication::applicationFilePath();
	std::string sPath = strPath.toStdString();
		
	return sPath;
}

std::string CLogFile::FileNameByDate(const QDate& date, bool bCleanUpMode)
{
	int iYear = date.year();
	int iDay = date.day();
	int iMonth = date.month(); 

	std::stringstream ssY;
	ssY << std::setw(2) << std::setfill('0') << iYear - 2000;

	std::stringstream ssD;
	ssD << std::setw(2) << std::setfill('0') << iDay;

	std::stringstream ssMn;
	ssMn << std::setw(2) << std::setfill('0') << iMonth;

	std::stringstream ss1;
	ss1 << ssD.str() << ssMn.str() << ssY.str();
	std::string sCurrentDate = ss1.str();

	std::stringstream ss2;
#ifdef Q_OS_WIN
	if (!bCleanUpMode)	// If work-mode:
		ss2 << "\\" << m_sApplicationName << "_" << sCurrentDate << ".log";
	else				// If Clean-up-mode:
		ss2 << m_sApplicationName << "_" << sCurrentDate << ".log";
#endif
#ifdef Q_OS_LINUX
    if (!bCleanUpMode)	// If work-mode:
        ss2 << "/" << m_sApplicationName << "_" << sCurrentDate << ".log";
    else				// If Clean-up-mode:
        ss2 << m_sApplicationName << "_" << sCurrentDate << ".log";
#endif
	std::string sLogiFileName = ss2.str();
	return sLogiFileName;
}

void CLogFile::SaveLogCore (std::string& sLog, bool bSaveMSec, LOG_MODES Category)
{
	std::string sPath = PrepareFullLogPath();

	QDateTime qdt = QDateTime::currentDateTime();

	QDate date = qdt.date();
	std::string sLogiFileName = sPath + FileNameByDate(date);
	
	QFile fileLog(sLogiFileName.c_str());

	if (!fileLog.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
	{
		QMessageBox::warning(NULL, "Avia Navi Client test Core", "Unable creating of Log-file !");
		return;
	}

	int iHour = qdt.time().hour();
	int iMinute = qdt.time().minute();
	int iSecond = qdt.time().second();
	int iMilliseconds = qdt.time().msec();

	std::stringstream ssH;
	ssH << std::setw(2) << std::setfill('0') << iHour;

	std::stringstream ssM;
	ssM << std::setw(2) << std::setfill('0') << iMinute;

	std::stringstream ssS;
	ssS << std::setw(2) << std::setfill('0') << iSecond;

	std::stringstream ssMS;
	if (bSaveMSec)	
		ssMS << std::setw(3) << std::setfill('0') << iMilliseconds;
		
	std::stringstream ss3;
	if (bSaveMSec)
		ss3 << ssH.str() << ":" << ssM.str() << ":" << ssS.str() << "." << ssMS.str();
	else
		ss3 << ssH.str() << ":" << ssM.str() << ":" << ssS.str();

	std::string sCurrentTime = ss3.str();
	
	std::stringstream ss4;
	
	std::string sCateg = GetCategory(Category);
	ss4 << sCurrentTime << sCateg << sLog << std::endl;
	
	std::string sTextInfo = ss4.str();

	fileLog.write(sTextInfo.c_str(), sTextInfo.length());
	fileLog.close();
}
