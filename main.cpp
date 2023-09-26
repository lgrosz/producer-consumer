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

    PipelineElement e1;
    PipelineElement e2;
    PipelineElement e3;
    PipelinePlusOner e4;
    PipelineDoubler e5;

    Pipeline pipeline { &e1, &e2, &e3, &e4, &e5 };

    for (int i = 0; i < 10; i++) {
        unsigned char c = QRandomGenerator::global()->bounded(10);
        qDebug() << "Inputting" << c;
        pipeline.input(c);
    }

    QObject::connect(&pipeline, &PipelineElement::output, [](unsigned char byte) { qDebug() << "Got" << byte; });

    return a.exec();
}
