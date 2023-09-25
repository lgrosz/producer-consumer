#include "pipeline.h"
#include <QDebug>

#include <unistd.h>

Pipeline::Pipeline(std::initializer_list<PipelineElement*> elements)
{
    int nPipes = elements.size() - 1;
    pipes = new fd_pair[nPipes];

    for (int i = 0; i < nPipes; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
        }
    }

    int counter = 0;
    for (PipelineElement *element : elements) {
        int inFd = counter < 1 ? -1 : pipes[counter-1][0];
        int outFd = counter < elements.size() - 1 ? pipes[counter][1] : -1;

        PipelineNode *node = new PipelineNode(element, inFd, outFd);

        element->moveToThread(node);
        nodes.append(node);

        counter++;
    }
}

Pipeline::~Pipeline()
{
    for (PipelineNode *node : nodes) {
        node->wait();
    }

    qDeleteAll(nodes);
    delete pipes;
}

void Pipeline::start()
{
    for (PipelineNode *node : nodes) {
        node->start();
    }
}

void Pipeline::quit()
{
    for (PipelineNode *node : nodes) {
        node->quit();
    }
}

