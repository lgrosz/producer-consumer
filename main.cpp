#include <QDebug>
#include <QRandomGenerator>
#include <QThread>

#include <unistd.h>
#include <sys/types.h>

/**
 * An abstraction for writing `typename T` to a unix pipe. For primitives, this
 * implementation works. For more complex types a concrete implementation
 * should be provided.
 */
template <typename T>
class Producer
{

public:
    explicit Producer(int writer, QObject *parent = nullptr)
        : writer(writer)
    {

    }

    /**
     * Method is called to produce `t` to the file descriptor.
     */
    void produce(T t)
    {
        write(writer, &t, sizeof(T));
    }

    /**
     * Method is called to indiciate nothing else will be produced.
     */
    void end()
    {
        close(writer);
    }

private:
    int writer;

};

/**
 * A thread which writes `n` random integers to `fd`.
 */
class RandomUIntProducer : public QThread
{

public:
    explicit RandomUIntProducer(unsigned int n, int fd, QObject *parent = nullptr)
        : n(n), producer(fd)
    {

    }

    void run() override
    {
        for (int i = 0; i < n; ++i) {
            producer.produce(QRandomGenerator::global()->bounded(10));
        }

        producer.end();
    }

private:
    const unsigned int n;
    Producer<quint32> producer;
};

/*
 * An abstraction for reading `typename T` to a unix pipe. For primitives, this
 * implementation works. For more complex types a concrete implementation
 * should be provided.
 */
template <typename T>
class Consumer
{

public:
    explicit Consumer(int reader, QObject *parent = nullptr)
        : reader(reader)
    {

    }

    ~Consumer()
    {
        close(reader);
    }

    bool consume(T *t)
    {
        return read(reader, t, sizeof(T)) > 0;
    }

private:
    int reader;

};

template <typename T>
class Logger : public QThread
{
public:
    explicit Logger(int reader, QObject *parent = nullptr)
        : QThread(parent), consumer(reader)
    {

    }

    void run() override
    {
        T t;

        int count = 0;
        while (consumer.consume(&t)) {
            qDebug() << ++count << t;
        }
    }

private:
    Consumer<T> consumer;

};

template <typename T>
class LogAndPassItOn : public QThread
{

public:
    explicit LogAndPassItOn(int reader, int writer, QObject *parent = nullptr)
        : QThread(parent), consumer(reader), producer(writer)
    {

    }

    void run() override
    {
        T t;

        int count = 0;
        while (consumer.consume(&t)) {
            qDebug() << ++count << t;
            producer.produce(t);
        }

        producer.end();
    }

private:
    Consumer<T> consumer;
    Producer<T> producer;

};

template <typename T>
class Doubler : public QThread
{

public:
    explicit Doubler(int reader, int writer, QObject *parent = nullptr)
        : QThread(parent), consumer(reader), producer(writer)
    {

    }

    void run() override
    {
        T t;

        int count = 0;
        while (consumer.consume(&t)) {
            producer.produce(t * 2);
        }

        producer.end();
    }

private:
    Consumer<T> consumer;
    Producer<T> producer;

};

int main(void)
{
    // TODO How do I hide this? Could I have some sort of class for the parts
    // of the pipeline that need methods like `setIn` and `setOut`? Then the
    // QThread idea could be private, `setIn`/`setOut` can invalidate the
    // class, and if they're appropriately set the thread gets started. Will
    // this cause data to be missed? I think I can then hide the pipe creation
    // in another class which contains the in and out implementations.

    int fd1[2];
    if (pipe(fd1) == -1) {
        perror("Could not create pipe");
        return 1;
    }

    int fd2[2];
    if (pipe(fd2) == -1) {
        perror("Could not create pipe");
        return 1;
    }

    int fd3[2];
    if (pipe(fd3) == -1) {
        perror("Could not create pipe");
        return 1;
    }

    RandomUIntProducer start(10, fd1[1]);
    LogAndPassItOn<quint32> logger(fd1[0], fd2[1]);
    Doubler<quint32> doubler(fd2[0], fd3[1]);
    Logger<quint32> end(fd3[0]);

    start.start();
    logger.start();
    doubler.start();
    end.start();

    start.wait();
    logger.wait();
    doubler.wait();
    end.wait();

    return 0;
}
