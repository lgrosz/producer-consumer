#ifndef PIPELINE_NODE_H
#define PIPELINE_NODE_H

#include <QThread>
#include <QSocketNotifier>


class PipelineElement;

class PipelineNode : public QObject
{
    Q_OBJECT

public:
    PipelineNode(int, int, QObject * = nullptr);

public slots:
    void writeByte(unsigned char);

signals:
    void readByte(unsigned char);

private:
    QSocketNotifier readSocketNotifier;
    int writeFd;

};

#endif // !PIPELINE_NODE_H
