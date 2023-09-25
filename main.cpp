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
        pipeline.input('A' + (QRandomGenerator::global()->bounded(26)));
    }

    QObject::connect(&pipeline, &PipelineElement::output, [](unsigned char byte) { qDebug() << byte; });

    return 0;
}
