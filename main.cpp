#include "appui.h"
#include "usbconnection.h"
#include "wsserver.h"

#include <QSerialPortInfo>
#include <QDebug>
#include <QApplication>
#include <QTimer>
#include <QThread>
#include <QSystemTrayIcon>
#include <QFile>
#include <QMessageBox>
#include <QMenu>
#include <QObject>

WSServer    wsServer;

static QTextStream logfile;
//static QTextStream lowlogfile;
static QTextStream cout(stdout);
//bool    dontLogNext = false;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QTextStream*    log = &logfile;
    /*if (QString(context.category) == "LowLevelTelnet")
        return ;*/
    //cout << msg;
    QString logString = QString("%6 %5 - %7: %1 \t(%2:%3, %4)").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function).arg(context.category, 20).arg(QDateTime::currentDateTime().toString(Qt::ISODate));
    switch (type)
    {
        case QtDebugMsg:
            *log << logString.arg("Debug");
            break;
        case QtCriticalMsg:
            *log << logString.arg("Critical");
            break;
        case QtWarningMsg:
            *log << logString.arg("Warning");
            break;
        case QtFatalMsg:
            *log << logString.arg("Fatal");
            *log<< "\n"; log->flush();
            QMessageBox::critical(nullptr, QObject::tr("Critical error"), msg);
            qApp->exit(1);
            break;
        case QtInfoMsg:
            *log << logString.arg("Info");
            break;
    }
    *log << "\n";
    log->flush();
    cout << QString("%1 : %2").arg(context.category, 20).arg(msg) << "\n";
    cout.flush();
}


void    startServer()
{
    //myTrayIcon->showMessage(QString("Webserver started"), QString("Webserver started"));
    if (!wsServer.start())
        qApp->exit(1);
}

int main(int ac, char *ag[])
{
    QApplication app(ac, ag);
    QFile   mlog(qApp->applicationDirPath() + "/log.txt");
    logfile.setDevice(&mlog);
    if (mlog.open(QIODevice::WriteOnly | QIODevice::Text))
        qInstallMessageHandler(myMessageOutput);
    QApplication::setApplicationName("QUsb2Snes");
    QApplication::setApplicationVersion("0.3");
    AppUi*  appUi = new AppUi();
    appUi->sysTray->show();
    QTimer::singleShot(200, &startServer);
    return app.exec();
}
