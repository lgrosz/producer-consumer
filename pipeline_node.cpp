#include "pipeline_node.h"

PipelineNode::PipelineNode(PipelineElement *element, int in_fd, int out_fd, QObject *parent)
    : QThread(parent)
{

}

