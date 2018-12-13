#include <QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCommandLineParser>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>
#include <QByteArray>
#include <QThread>
#include "zmqlistener.h"
#include "config.h"
#include "qmllogging.h"

_INITIALIZE_QMLLOGGING


//el::base::type::StoragePointer Config::getLogStorage() {return el::Helpers::storage();}

QString msgBoxStr = "import QtQuick 2.8\nimport QtQuick.Window 2.2"
"\nWindow { visible:true\nwidth: 600\nheight: 360\nRectangle {anchors.fill: parent"
"\nMouseArea {anchors.fill: parent\nonClicked: { Qt.quit();}}\nText {anchors.centerIn: parent"
"\ntext: \"%1\"}}}";

QCoreApplication *a;

void msgBox(const QString& str) {
    QQmlApplicationEngine engine;
    engine.loadData(msgBoxStr.arg(str).toLatin1());
    //engine.load(QUrl(QStringLiteral("err.qml")));
    a->exec();
}

/*ZMQConstruct* socks=0;
ZMQListener* listener=0;*/
QObject *qmlroot = 0;
QString cfgData;

void objectCreated(QObject * object, const QUrl & url) {
    Q_UNUSED(url);
    if (!qmlroot) {
        qmlroot = object;
    }
}

int main(int argc, char *argv[])
{
    START_EASYLOGGINGPP(argc,argv);
    QString cfg_name = "config.json";
    QString cfg_field;
    QString qml_file = "main.qml";

    QApplication app(argc, argv);
    a = &app;
    QCoreApplication::setOrganizationName("GCZO");
    QCoreApplication::setOrganizationDomain("meteo.gov.ua");
    QCoreApplication::setApplicationName("ZeroMQ QML Player");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("QML player with ZeroMQ interface to QML code");

    const QCommandLineOption helpOpt = parser.addHelpOption();
    const QCommandLineOption verOpt = parser.addVersionOption();
    parser.addPositionalArgument("qml", "Program file name. Default: main.qml","qml [config [prop]]");
    parser.addPositionalArgument("config", "Configuration file name. Default: config.json"," ");
    parser.addPositionalArgument("prop", "Property with this app config settings"," ");

    QCommandLineOption cfgCodePage("cfg-cp","Config file codepage (default UTF-8)", "codepage", "UTF-8");
    parser.addOption(cfgCodePage);

    QCommandLineOption logFileNameOption("logfile-name","The log file name", "filename", "log.conf");
    parser.addOption(logFileNameOption);

    QCommandLineOption logLogVerbose(QStringList() << "v" << "verbose", "LOG: Activates maximum verbosity");
    parser.addOption(logLogVerbose);

    QCommandLineOption logLogVerboseLevel("v","LOG: Activates verbosity upto verbose level (valid range: 0-9)","level");
    parser.addOption(logLogVerboseLevel);

    QCommandLineOption logLogVModule("vmodule", "LOG: vmodule option","MODULE_NAME");
    parser.addOption(logLogVModule);

    QCommandLineOption logLogFlags("logging-flags", "LOG: Sets logging flag","value");
    parser.addOption(logLogFlags);

    QCommandLineOption logLogFileName("default-log-file", "LOG: Sets default log file for existing and future loggers","filename");
    parser.addOption(logLogFileName);
    if (!parser.parse(QCoreApplication::arguments())) {
        msgBox(parser.errorText());
        return -1;
    }

    if (parser.isSet(verOpt)) {
        msgBox(QString("<p><b>%1</b></p><p>Version:%2</p>").arg(QCoreApplication::applicationName(),QCoreApplication::applicationVersion()));
        return 0;
    }
    if (parser.isSet(helpOpt)) {
        msgBox(parser.helpText().replace('\\',"\\\\"));
        return 0;
    }
    QString logConfName = parser.value(logFileNameOption);
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    //defaultConf.setGlobally(el::ConfigurationType::Filename,"zmqplayer.log");
    //el::Logger* l = el::Loggers::getLogger("default");
    //qDebug()<<"log filename"<<l->typedConfigurations()->filename(el::Level::Info).c_str();
    el::Loggers::setDefaultConfigurations(defaultConf);
    el::Loggers::configureFromGlobal(logConfName.toStdString().c_str());

    const QStringList args = parser.positionalArguments();

    if (args.count()>0) { qml_file = args[0];
    }
    if (args.count()>1) { cfg_name = args[1];
    }
    if (args.count()>2) { cfg_field = args[2];
    }

    LOG(DEBUG)<<cfg_name<<cfg_field;

    Config config;
    if (!config.getConfig(cfg_name,cfg_field,parser.value(cfgCodePage).toLatin1().data())) {
        msgBox(QString("Can not parse config file <b>%1</b>.").arg(cfg_name));
        return -1;
    }
//    QObject::connect(&app,SIGNAL(commitDataRequest(QSessionManager&)),&config,SLOT(commitData(QSessionManager&)))       ;
    QObject::connect(&app,SIGNAL(aboutToQuit()),&config,SLOT(save()));

    ZMQ zmq;
    ZMQProxy zmq_prx;

    QObject::connect(&zmq,SIGNAL(recv(QString,QVariant)),&zmq_prx,SLOT(recv_prx(QString,QVariant)));
    QObject::connect(&zmq,SIGNAL(errConfigure(int,QString)),&zmq_prx,SLOT(errConfigure_prx(int,QString)));
    QObject::connect(&zmq,SIGNAL(errSend(int,QString)),&zmq_prx,SLOT(errSend_prx(int,QString)));
    QObject::connect(&zmq,SIGNAL(errAdd(int,QString)),&zmq_prx,SLOT(errAdd_prx(int,QString)));
    QObject::connect(&zmq,SIGNAL(okConfigure()),&zmq_prx,SLOT(okConfigure_prx()));

    QObject::connect(&zmq_prx,SIGNAL(configure_prx(QVariantMap)),&zmq,SLOT(configure(QVariantMap)));
    QObject::connect(&zmq_prx,SIGNAL(addListener_prx(QString)),&zmq,SLOT(addListener(QString)));
    QObject::connect(&zmq_prx,SIGNAL(removeListener_prx(QString)),&zmq,SLOT(removeListener(QString)));
    QObject::connect(&zmq_prx,SIGNAL(send_prx(QString,QVariant)),&zmq,SLOT(send(QString,QVariant)));

    QThread t;
    QObject::connect(&app,SIGNAL(aboutToQuit()),&t,SLOT(quit()));
    zmq.moveToThread(&t);
    t.start();

    QQmlApplicationEngine engine;
    el::qml::QmlLogging::registerNew(engine.rootContext());
    engine.rootContext()->setContextProperty("config",&config);
    engine.rootContext()->setContextProperty("zmq",&zmq_prx);


    /*if (!cfg.contains("zmq")) {
        msgBox(QString("No <b>'zmq'</b> section in configuration"));
        qCritical()<<QString("No 'zmq' section in configuration");
        return -1;
    } else {
        auto zmqcfg = cfg["zmq"].toObject();
        if (!zmqcfg.contains("sockets")) {
            msgBox(QString("No sockets described in <b>'zmq'</b> section in configuration"));
            qCritical()<<QString("No sockets described in <b>'zmq'</b> section in configuration");
            return -1;
        } else {
            socks = new ZMQConstruct(listener);
            if (!socks->addJson(zmqcfg["sockets"].toObject())) {
                msgBox(QString("Error in the socket description"));
                qCritical()<<QString("Error in the socket description");
                return -1;
            }
        }
    }*/
    QObject::connect(&engine,&QQmlApplicationEngine::objectCreated,objectCreated);
    LOG(DEBUG)<<"app load";
    engine.load(QUrl(qml_file));
    LOG(DEBUG)<<"app exec";
    return app.exec();
}
