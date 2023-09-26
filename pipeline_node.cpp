#include "pipeline_node.h"
#include "pipeline_element.h"
#include <unistd.h>
#include <QDebug>

PipelineNode::PipelineNode(int readFd, int writeFd, QObject *parent)
    : QObject(parent), readSocketNotifier(readFd, QSocketNotifier::Read), writeFd(writeFd)
{
    connect(&readSocketNotifier, &QSocketNotifier::activated, this, [&]() {
        unsigned char byte;

        if (read(readSocketNotifier.socket(), &byte, 1) < 1) {
            qWarning() << "Read no bytes";
        } else {
            emit readByte(byte);
        }
    });
}

void PipelineNode::writeByte(unsigned char byte)
{
    if (write(writeFd, &byte, 1) < 1) {
        qWarning() << "Failed to write byte ";
    }
}
