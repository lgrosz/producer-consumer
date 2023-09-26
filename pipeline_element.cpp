#include "pipeline_element.h"
#include <QThread>
#include <QDebug>

PipelineElement::PipelineElement(QObject *parent)
    : QObject (parent)
{

}

void PipelineElement::input(QByteArray byte)
{
    emit output(byte);
}

void PipelineElement::endInput()
{
    emit endOutput();
}

