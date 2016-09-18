#ifndef MAINUI_H
#define MAINUI_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include "huffman.h"

class MainUi : public QWidget
{
    Q_OBJECT

public:
    MainUi(QWidget *parent = 0);

private slots:
    void getSourceFile();
    void getObjectFile();

    void doCompression();
    void deDecompression();
    
private:
    Huffman *huffman;

    QLabel *sourceFileLabel;
    QLabel *objectFileLabel;
    QLabel *progressTitleLabel;

    QLineEdit *sourceFileLineEdit;
    QLineEdit *objectFileLineEdit;
    QProgressBar *progressBar;

    QPushButton *openSourceFileButton;
    QPushButton *openObjectFileButton;

    QPushButton *compressionPushButton;
    QPushButton *decompressionPushButton;
};

#endif // MAINUI_H
