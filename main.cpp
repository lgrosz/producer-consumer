#include <QCoreApplication>
#include <QDebug>
#include <QRandomGenerator>
#include "pipeline.h"

//class PipelineDoubler : public PipelineElement
//{
//
//public:
//    void input(QByteArray byte) override
//    {
//        emit output(byte * 2);
//    }
//
//};
//
//class PipelinePlusOner : public PipelineElement
//{
//
//public:
//    void input(QByteArray byte) override
//    {
//        emit output(byte + 1);
//    }
//
//};

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);

    PipelineElement e1;
    PipelineElement e2;

    Pipeline pipeline { &e1, &e2 };

    QObject::connect(&pipeline, &PipelineElement::output, [](QByteArray byte) { qDebug() << "Got" << byte; });

    QObject::connect(&pipeline, &PipelineElement::endOutput, &a, &QCoreApplication::quit, Qt::QueuedConnection);

    for (int i = 0; i < 10; i++) {
        char c = QRandomGenerator::global()->bounded(10);
        QByteArray arr;
        arr.append(c);
        qDebug() << "Inputting" << arr;
        pipeline.input(arr);
    }

    pipeline.endInput();

    return a.exec();
}
