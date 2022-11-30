#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>
#include <QFileDialog>
#include <QDir>
#include <QFile>

#include "widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;

    QLineEdit* widthLineEdit = new QLineEdit("256", &w);
    QLineEdit* heightLineEdit = new QLineEdit("256", &w);
    QPushButton* button = new QPushButton("Load", &w);
    QLabel* imageLabel = new QLabel(&w);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(widthLineEdit);
    layout->addWidget(heightLineEdit);
    layout->addStretch(1);
    layout->addWidget(button);

    QVBoxLayout* mainLayout = new QVBoxLayout(&w);
    mainLayout->addLayout(layout);
    mainLayout->addWidget(imageLabel);
    /*
    QObject::connect(button, &QPushButton::clicked, [&](){
        QString fileName = QFileDialog::getOpenFileName(0,
                                  "Open a file", QDir::homePath(), "raw file(*.raw)");
*/
    QObject::connect(button, &QPushButton::clicked, [&](){
        QString fileName = QFileDialog::getOpenFileName(0, "Open a file", QDir::homePath(),
                                                        "Image file(*.raw *.pbm *.pgm *.ppm *.bmp *.jpg *.png)");
        if(!fileName.length()) return;

        QFile file(fileName);
        file.open(QFile::ReadOnly);
        QByteArray byteArray = file.readAll();
        file.close();

        int width = widthLineEdit->text().toInt();
        int height = heightLineEdit->text().toInt();
        uchar* data = (uchar*)(byteArray.data());

        QImage image = QImage(data, width, height, QImage::Format_Grayscale8);

        QFileInfo fileInfo(fileName);
        if(fileInfo.suffix() == "raw")
            imageLabel->setPixmap(QPixmap::fromImage(image, Qt::AutoColor));
        else
            imageLabel->setPixmap(QPixmap(fileName).scaled(width,height));

        w.setWindowTitle(fileName);

    });

    w.show();
    return a.exec();
}
