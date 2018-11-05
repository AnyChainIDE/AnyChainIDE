#include "UpdateProcess.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QProcess>

#include "UpdateProgressUtil.h"
#include "UpdateNetWork.h"

#ifdef WIN32
static const QString UPDATE_DOC_NAME = "ide_upgrade_win.xml";
#else
static const QString UPDATE_DOC_NAME = "ide_upgrade_mac.xml";
#endif
static const QString UPDATE_DIR_NAME = "updatetemp";//更新主目录
static const QString COPY_DIR_NAME = "copy";//更新器目录

class UpdateProcess::DataPrivate
{
public:
    DataPrivate(const QString &server)
        :updateNetwork(new UpdateNetWork())
        ,networkManager(new QNetworkAccessManager())
        ,serverUrl(server)
        ,downloadPath(QCoreApplication::applicationDirPath() + QDir::separator() + UPDATE_DIR_NAME)
        ,localVersionData(std::make_shared<VersionInfo>())
        ,serverVersionData(std::make_shared<VersionInfo>())
        ,isWrongHappened(false)
    {
    }
    ~DataPrivate()
    {
        if(updateNetwork)
        {
            delete updateNetwork;
            updateNetwork = nullptr;
        }
        if(networkManager)
        {
            delete networkManager;
            networkManager = nullptr;
        }
    }
public:
    UpdateNetWork *updateNetwork;
    QNetworkAccessManager   *networkManager;

    QString serverUrl;

    VersionInfoPtr localVersionData;//本地文件解析结果
    VersionInfoPtr serverVersionData;//服务器下载的文件解析结果

    QString downloadPath;

    QList<DownLoadData> updateList;//需要更新的列表

    bool isWrongHappened;
};

UpdateProcess::UpdateProcess(const QString &serverUrl,QObject *parent)
    : QObject(parent)
    ,_p(new DataPrivate(serverUrl))
{
    InitData();
}

UpdateProcess::~UpdateProcess()
{
    delete _p;
    _p = nullptr;
}

void UpdateProcess::checkUpdate()
{
    //设置当前没有错误
    _p->isWrongHappened = false;

    disconnect(_p->updateNetwork,&UpdateNetWork::TaskEmpty,this,&UpdateProcess::DownloadEmptySlot);
    //先清空下载的数据
    UpdateProgressUtil::deleteDir(_p->downloadPath);

    //本地版本文件解析
    UpdateProgressUtil::ParseXmlPath(QCoreApplication::applicationDirPath() + QDir::separator() + UPDATE_DOC_NAME,
                                     _p->localVersionData);

    //获取最新的配置
    GetLatestVersionInfo();
}

void UpdateProcess::startDownload()
{    //开始下载
    //链接最终下载完毕信号
    connect(_p->updateNetwork,&UpdateNetWork::TaskEmpty,this,&UpdateProcess::DownloadEmptySlot);
    _p->updateNetwork->DownLoadFile(_p->updateList);
}

void UpdateProcess::startUpdate()
{
    //关闭主程序，启动复制程序
    startDownload();

}

void UpdateProcess::DownLoadVersionConfigFinsihed()
{
    disconnect(_p->updateNetwork,&UpdateNetWork::DownLoadFinish,this,&UpdateProcess::DownLoadVersionConfigFinsihed);

    //下载完配置文件后，对比本地、服务器配置，列出需要更新的配置项
    //服务端下载的版本文件解析
    UpdateProgressUtil::ParseXmlPath(_p->downloadPath + QDir::separator() + UPDATE_DOC_NAME,_p->serverVersionData);

    if(_p->serverVersionData->version.isEmpty())
    {
        return;//说明出错，没必要进行了
    }

    //对比分析出需要下载的版本信息及目录
    UpdateProgressUtil::ExtractUpdateData(_p->localVersionData,_p->serverVersionData,
                                          _p->downloadPath+QDir::separator()+COPY_DIR_NAME,_p->downloadPath,
                                          _p->updateList);
    //发送版本信息
    emit NewstVersionSignal(_p->updateList.empty()?"":_p->serverVersionData->version);
}

void UpdateProcess::DownloadEmptySlot()
{
    //下载完毕，发出更新完毕的信号
    if(_p->isWrongHappened) return;
    UpdateProgressUtil::copyDir(_p->downloadPath+QDir::separator()+COPY_DIR_NAME,
                                QCoreApplication::applicationDirPath());
    UpdateProgressUtil::deleteDir(_p->downloadPath+QDir::separator()+COPY_DIR_NAME);

    emit updateFinish();

}

void UpdateProcess::DownloadWrongSlot(const QString &fileName)
{
    _p->isWrongHappened = true;
    qDebug()<<fileName<<"download error";
    emit updateWrong();
}

void UpdateProcess::GetLatestVersionInfo()
{
    //下载当前服务器版本
    DownLoadData up;
    up.fileName = UPDATE_DOC_NAME;
    up.url = _p->serverUrl + up.fileName;//"http://192.168.1.161/down/blocklink_wallet_upgrade.xml";//测试用，本地文件
    up.filePath = _p->downloadPath + QDir::separator() + up.fileName;
    connect(_p->updateNetwork,&UpdateNetWork::DownLoadFinish,this,&UpdateProcess::DownLoadVersionConfigFinsihed);

    _p->updateNetwork->DownLoadFile(up);
}

void UpdateProcess::InitData()
{
    connect(_p->updateNetwork,&UpdateNetWork::DwonLoadWrong,this,&UpdateProcess::DownloadWrongSlot);
}



