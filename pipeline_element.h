#ifndef PIPELINE_ELEMENT_H
#define PIPELINE_ELEMENT_H

#include <QObject>

class PipelineElement : public QObject
{
    Q_OBJECT

public:
    PipelineElement(QObject * = nullptr);

public slots:
    virtual void input(QByteArray);
    virtual void endInput();

signals:
    void output(QByteArray);
    void endOutput();

};

#endif // !PIPELINE_ELEMENT_H
