#include "pipeline_element.h"
#include <QThread>
#include <QDebug>

PipelineElement::PipelineElement(QObject *parent)
    : QObject (parent)
{

}

void PipelineElement::input(unsigned char byte)
{
    emit output(byte);
}

