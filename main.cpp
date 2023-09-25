#include "pipeline.h"

int main(void)
{
    PipelineElement e1;
    PipelineElement e2;
    PipelineElement e3;

    Pipeline pipeline { &e1, &e2, &e3 };

    pipeline.start();
    pipeline.quit();

    return 0;
}
