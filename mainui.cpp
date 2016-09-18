#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QString>
#include "mainui.h"
#include "huffman.h"

MainUi::MainUi(QWidget *parent)
    : QWidget(parent)
{
    huffman = new Huffman;

    // 控件
    sourceFileLabel    = new QLabel("源文件", this);
    objectFileLabel    = new QLabel("目标文件", this);
    progressTitleLabel = new QLabel("处理进度", this);

    sourceFileLineEdit = new QLineEdit(this);
    objectFileLineEdit = new QLineEdit(this);
    progressBar        = new QProgressBar(this);

    openSourceFileButton    = new QPushButton("选择源文件", this);
    openObjectFileButton    = new QPushButton("选择目标文件", this);

    compressionPushButton   = new QPushButton("压缩(&C)", this);
    decompressionPushButton = new QPushButton("解压(&E)", this);

    // connect
    connect(openSourceFileButton, SIGNAL(clicked(bool)), this, SLOT(getSourceFile()));
    connect(openObjectFileButton, SIGNAL(clicked(bool)), this, SLOT(getObjectFile()));

    connect(compressionPushButton, SIGNAL(clicked(bool)), this, SLOT(doCompression()));
    connect(decompressionPushButton, SIGNAL(clicked(bool)), this, SLOT(deDecompression()));

    connect(huffman, SIGNAL(progressChanged(int)), progressBar, SLOT(setValue(int)));

    // 布局
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setColumnStretch(0, 0);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 0);

    gridLayout->addWidget(sourceFileLabel, 0, 0);
    gridLayout->addWidget(objectFileLabel, 1, 0);
    gridLayout->addWidget(progressTitleLabel, 2, 0);

    gridLayout->addWidget(sourceFileLineEdit, 0, 1);
    gridLayout->addWidget(objectFileLineEdit, 1, 1);
    gridLayout->addWidget(progressBar, 2, 1, 1, 2);

    gridLayout->addWidget(openSourceFileButton, 0, 2);
    gridLayout->addWidget(openObjectFileButton, 1, 2);

    QHBoxLayout *buttonsBoxLayout = new QHBoxLayout;
    buttonsBoxLayout->addWidget(compressionPushButton, 1, Qt::AlignRight);
    buttonsBoxLayout->addWidget(decompressionPushButton);

    QVBoxLayout *mainBoxLayout = new QVBoxLayout;
    mainBoxLayout->addLayout(gridLayout);
    mainBoxLayout->addLayout(buttonsBoxLayout);

    setLayout(mainBoxLayout);
}

void MainUi::getSourceFile()
{
    QString sourceFileName = QFileDialog::getOpenFileName(this, "打开源文件");
    sourceFileLineEdit->setText(sourceFileName);
}

void MainUi::getObjectFile()
{
    QString objectFileName = QFileDialog::getSaveFileName(this, "打开目标文件");
    objectFileLineEdit->setText(objectFileName);
}

void MainUi::doCompression()
{
    QString sourceFileName = sourceFileLineEdit->text();
    QString objectFileName = objectFileLineEdit->text();

    huffman->init();
    huffman->setInFile(sourceFileName.toStdString());
    huffman->setOutFile(objectFileName.toStdString());

    huffman->compression();
}

void MainUi::deDecompression()
{
    QString sourceFileName = sourceFileLineEdit->text();
    QString objectFileName = objectFileLineEdit->text();

    huffman->init();
    huffman->setInFile(sourceFileName.toStdString());
    huffman->setOutFile(objectFileName.toStdString());

    huffman->decompression();
}
