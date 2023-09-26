#include <QCoreApplication>
#include <QDebug>
#include <QRandomGenerator>
#include "pipeline.h"

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);

    PipelineElement e1;
    PipelineElement e2;
    PipelineElement e3;

    Pipeline pipeline { &e1, &e2, &e3 };

    for (int i = 0; i < 10; i++) {
        unsigned char c = 'A' + (QRandomGenerator::global()->bounded(26));
        qDebug() << "Inputting" << c;
        pipeline.input(c);
    }

    QObject::connect(&pipeline, &PipelineElement::output, [](unsigned char byte) { qDebug() << "Got" << byte; });

    return a.exec();
}
