#include "NewFileDialog.h"
#include "ui_NewFileDialog.h"

#include <QFileInfo>
#include <QDebug>
#include "IDEUtil.h"

class NewFileDialog::DataPrivate
{
public:
    DataPrivate(const QString &topDir,const QStringList &docs)
        :rootDir(topDir)
        ,types(docs)
    {
        IDEUtil::GetAllFileFolder(topDir,dirList);
    }
public:
    QStringList dirList;
    QString rootDir;
    QStringList types;

    QString newFilePath;
};

NewFileDialog::NewFileDialog(const QString &topDir,const QStringList &docs,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFileDialog),
    _p(new DataPrivate(topDir,docs))
{
    ui->setupUi(this);
    InitWidget();
}

NewFileDialog::~NewFileDialog()
{
    delete _p;
    delete ui;
}

const QString &NewFileDialog::getNewFilePath() const
{
    return _p->newFilePath;
}

void NewFileDialog::TextChanged(const QString &text)
{
    ValidFile();
}

void NewFileDialog::ConfirmSlot()
{
    bool isValid = false;
     foreach(QString var,_p->types)
     {
         if(ui->lineEdit->text().endsWith(var))
         {
             isValid = true;
             break;
         }
     }
     if(!isValid)
     {
         ui->lineEdit->setText(ui->lineEdit->text()+_p->types.front());
     }
     _p->newFilePath = ui->comboBox->currentText() + "/"+ui->lineEdit->text();

     qDebug()<<_p->newFilePath;
     IDEUtil::TemplateFile(_p->newFilePath);
     close();
}

void NewFileDialog::CancelSlot()
{
    _p->newFilePath.clear();
    close();
}

void NewFileDialog::InitWidget()
{
    ui->label_tip->setVisible(false);
    ui->comboBox->addItems(_p->dirList);

    connect(ui->lineEdit,&QLineEdit::textChanged,this,&NewFileDialog::TextChanged);
    connect(ui->okBtn,&QToolButton::clicked,this,&NewFileDialog::ConfirmSlot);
    connect(ui->cancelBtn,&QToolButton::clicked,this,&NewFileDialog::CancelSlot);
    connect(ui->comboBox,static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),this,&NewFileDialog::ValidFile);

}

void NewFileDialog::ValidFile()
{
    QRegExp regx("[\\\\/:*?\"<>|]");
    if(ui->lineEdit->text().indexOf(regx) >= 0)
    {
        //文件名不合法
        ui->label_tip->setText(tr("invalid name!"));
        ui->label_tip->setVisible(true);
        ui->okBtn->setEnabled(false);
    }
    else if(IDEUtil::isFileExist(ui->lineEdit->text(),ui->comboBox->currentText()))
    {//文件已存在
        ui->label_tip->setText(tr("already exist!"));
        ui->label_tip->setVisible(true);
        ui->okBtn->setEnabled(false);
    }
    else
    {
        ui->label_tip->setVisible(false);
        ui->okBtn->setEnabled(true);
    }
}
