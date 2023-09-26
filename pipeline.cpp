#include "pipeline.h"
#include <QDebug>
#include <QThread>

struct PipelineNodeContainer
{
    QThread *thread;
    PipelineElement *element;
};

Pipeline::Pipeline(std::initializer_list<PipelineElement*> elements)
{
    PipelineElement *previous = &root;

    for (PipelineElement *current : elements) {
        QThread *thread = new QThread(this);
        current->moveToThread(thread);
        thread->start();

        connect(current, &PipelineElement::endOutput, thread, &QThread::quit, Qt::QueuedConnection);
        connect(previous, &PipelineElement::output, current, &PipelineElement::input, Qt::QueuedConnection);
        connect(previous, &PipelineElement::endOutput, current, &PipelineElement::endInput, Qt::QueuedConnection);

        nodes.append({ thread, current });

        previous = current;
    }

    connect(previous, &PipelineElement::output, this, &PipelineElement::output);
    connect(previous, &PipelineElement::endOutput, this, &PipelineElement::endOutput);
}

Pipeline::~Pipeline()
{
    for (PipelineNodeContainer node : nodes) {
        node.thread->wait();
    }

    // TODO Cleanup threads?
}

void Pipeline::input(unsigned char byte)
{
    root.input(byte);
}

void Pipeline::endInput()
{
    root.endInput();
}
