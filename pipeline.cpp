#include "pipeline.h"
#include <QDebug>

#include <unistd.h>

typedef int fd_pair[2];

struct PipelineNodeContainer
{
    QThread *thread;
    PipelineElement *element;
    PipelineNode *node;
};

Pipeline::Pipeline(std::initializer_list<PipelineElement*> elements)
{
    fd_pair left_pipe;
    fd_pair right_pipe;

    if (pipe(left_pipe) == -1) {
        qWarning() << "Failed to create left pipe";
    }

    // In case there are no elements, then we will just let the pipe go through.
    writeFd = left_pipe[1];

    for (PipelineElement *element : elements) {
        QThread *thread = new QThread(this);

        if (pipe(right_pipe) == -1) {
            qWarning() << "Failed to create right pipe";
        }

        // Setup node to relay the input of the left pipe to the input of the
        // element and to relay the output of the element to the output of the
        // right pipe
        PipelineNode *node = new PipelineNode(left_pipe[0], right_pipe[1]);
        connect(node, &PipelineNode::readByte, element, &PipelineElement::input);
        connect(element, &PipelineElement::output, node, &PipelineNode::writeByte);

        element->moveToThread(thread);
        node->moveToThread(thread);
        thread->start();

        nodes.append({ thread, element, node });

        // Right pipe is the left pipe for the next pass
        memcpy(left_pipe, right_pipe, 2 * sizeof(int));
    }

    readFd = left_pipe[0];
    readNotifier = new QSocketNotifier(readFd, QSocketNotifier::Read, this);
    connect(readNotifier, &QSocketNotifier::activated, [&]() {
        unsigned char byte;
        if (read(readFd, &byte, 1) < 1) {
            qWarning() << "Failed to read byte";
        }

        emit output(byte);
    });
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
    if (write(writeFd, &byte, 1) < 1) {
        qWarning() << "Failed to write";
    }
}
