#include "pipeline_node.h"
#include <QDebug>

PipelineNode::PipelineNode(PipelineElement *element, int in_fd, int out_fd, QObject *parent)
    : QThread(parent)
{
    qDebug() << "PipelineNode" << in_fd << out_fd;
}

void PipelineNode::run()
{
    qDebug() << QThread::currentThreadId() << "PipelineNode::run";
}

