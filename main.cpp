#include <QCoreApplication>
#include <QDebug>
#include <QRandomGenerator>
#include "pipeline.h"

class PipelineDoubler : public PipelineElement
{

public:
    void input(unsigned char byte) override
    {
        emit output(byte * 2);
    }

};

class PipelinePlusOner : public PipelineElement
{

public:
    void input(unsigned char byte) override
    {
        emit output(byte + 1);
    }

};

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);

    PipelinePlusOner e1;
    PipelineDoubler e2;

    Pipeline pipeline { &e1, &e2 };

    QObject::connect(&pipeline, &PipelineElement::output, [](unsigned char byte) { qDebug() << "Got" << byte; });

    QObject::connect(&pipeline, &PipelineElement::endOutput, &a, &QCoreApplication::quit, Qt::QueuedConnection);

    for (int i = 0; i < 10; i++) {
        unsigned char c = QRandomGenerator::global()->bounded(10);
        qDebug() << "Inputting" << c;
        pipeline.input(c);
    }

    pipeline.endInput();

    return a.exec();
}
