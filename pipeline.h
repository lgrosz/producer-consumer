#ifndef PIPELINE_H
#define PIPELINE_H

#include <QList>
#include <initializer_list>
#include "pipeline_element.h"
#include "pipeline_node.h"

struct PipelineNodeContainer;

// TODO How cool would it be for a Pipeline to also be a PipelineElement? Then
// you could insert a pipeline into a pipeline. Also.. maybe PipelineNode
// should be private, so nobody can create one?
class Pipeline : public PipelineElement
{

public:
    Pipeline(std::initializer_list<PipelineElement*>);
    ~Pipeline();

    void input(unsigned char) override;

private:
    int readFd;
    int writeFd;
    QList<PipelineNodeContainer> nodes;
    QSocketNotifier *readNotifier;

};

#endif // !PIPELINE_H
