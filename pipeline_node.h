#ifndef PIPELINE_NODE_H
#define PIPELINE_NODE_H

#include <QThread>
#include <QSocketNotifier>


class PipelineElement;

class PipelineNode : public QThread
{
    Q_OBJECT

public:
    PipelineNode(PipelineElement *, int, int, QObject * = nullptr);

    //void run() override;

private slots:
    void readSocket(int);

private:
    PipelineElement *element;
    QSocketNotifier *in_notifier;
    int out_fd;

};

#endif // !PIPELINE_NODE_H
