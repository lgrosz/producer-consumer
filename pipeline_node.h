#ifndef PIPELINE_NODE_H
#define PIPELINE_NODE_H

#include <QThread>

class PipelineElement;

class PipelineNode : public QThread
{

public:
    PipelineNode(PipelineElement *, int, int, QObject * = nullptr);

    void run() override;

};

#endif // !PIPELINE_NODE_H
