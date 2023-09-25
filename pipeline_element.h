#ifndef PIPELINE_ELEMENT_H
#define PIPELINE_ELEMENT_H

#include <QObject>

class PipelineElement : public QObject
{
    Q_OBJECT

public:
    PipelineElement(QObject * = nullptr);

public slots:
    virtual void input(unsigned char);

signals:
    void output(unsigned char);

};

#endif // !PIPELINE_ELEMENT_H
