#include "pipeline.h"
#include <QDebug>

#include <unistd.h>

Pipeline::Pipeline(std::initializer_list<PipelineElement*> elements)
{
    assert(elements.size() > 0);

    first = *elements.begin();
    last = *(elements.end() - 1);

    int nPipes = elements.size() - 1;
    pipes = new fd_pair[nPipes];

    for (int i = 0; i < nPipes; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
        }

        qDebug() << "pipe" << pipes[i][0] << pipes[i][1];
    }

    int counter = 0;
    for (PipelineElement *element : elements) {
        int inFd = counter < 1 ? -1 : pipes[counter-1][0];
        int outFd = counter < elements.size() - 1 ? pipes[counter][1] : -1;

        PipelineNode *node = new PipelineNode(element, inFd, outFd);

        element->moveToThread(node);
        nodes.append(node);
        node->start();

        counter++;
    }

    // TODO Start nodes
    connect(last, &PipelineElement::output, this, &PipelineElement::output);
}

Pipeline::~Pipeline()
{
    for (PipelineNode *node : nodes) {
        node->wait();
    }

    qDeleteAll(nodes);
    delete pipes;
}

void Pipeline::input(unsigned char byte)
{
    first->input(byte);
}
