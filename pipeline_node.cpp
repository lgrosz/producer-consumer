#include "pipeline_node.h"
#include "pipeline_element.h"
#include <unistd.h>
#include <QDebug>

PipelineNode::PipelineNode(PipelineElement *element, int in_fd, int out_fd, QObject *parent)
    : QThread(parent), element(element), in_notifier(nullptr), out_fd(out_fd)
{
    if (0 < in_fd) {
        qDebug() << "setting up notifier for file descriptor" << in_fd;
        in_notifier = new QSocketNotifier(in_fd, QSocketNotifier::Read, this);

        connect(in_notifier, &QSocketNotifier::activated, this, [&]() {
            qDebug() << "notifier activated";
        });

        in_notifier->setEnabled(true);
    }

    if (0 < out_fd) {
        qDebug() << "setting up element to write to" << out_fd;
        connect(element, &PipelineElement::output, this, [=](unsigned char byte) {
            qDebug() << "Got output, writing to" << out_fd;
            write(out_fd, &byte, 1);
        });
    }
}

void PipelineNode::readSocket(int)
{
    qDebug() << "PipelineNode::readSocket";
    if (in_notifier) {
        qDebug() << in_notifier->socket();
    }
}

// TODO There needs to be some way of killing the thread
//void PipelineNode::run()
//{
//    // TODO This blocks, the connections above won't run!
//    qDebug() << QThread::currentThreadId() << "PipelineNode::run";
//
//    unsigned char byte;
//    while (read(in_fd, &byte, 1) > 0) {
//        element.input(byte);
//    }
//
//    qDebug() << QThread::currentThreadId() << "Finished reading";
//    close (in_fd);
//
//    // TODO This should be deferred or the element should signal that it is
//    // finished.
//    close (out_fd);
//}

