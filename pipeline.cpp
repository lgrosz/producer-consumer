#include "pipeline.h"
#include <QDebug>

#include <unistd.h>

Pipeline::Pipeline(std::initializer_list<PipelineElement*> elements)
{
    const int nPipes = elements.size() - 1;
    int *pipes = new int(nPipes * 2);

    qDebug() << "nPipes:" << nPipes;

    for (int i = 0; i < nPipes; i++) {
        pipe(pipes + i * 2 * sizeof(int));
    }

    int counter = 0;
    for (PipelineElement *element : elements) {
        PipelineNode *node = new PipelineNode(element, pipes[counter], pipes[counter + 1]);

        element->moveToThread(node);
        nodes.append(node);

        counter = counter + 2;
    }
}

Pipeline::~Pipeline()
{
    for (PipelineNode *node : nodes) {
        node->wait();
    }

    qDeleteAll(nodes);
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

