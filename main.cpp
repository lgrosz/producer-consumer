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
class Producer : public QThread
{

public:
    explicit Producer(QObject *parent = nullptr)
        : QThread(parent), fd(0), ok(false)
    {

    }

    void to(int fd)
    {
        ok = true;
        this->fd = fd;
    }

    /**
     * Method is called to produce `t` to the file descriptor.
     */
    void produce(T t)
    {
        if (ok) {
            qDebug() << "produce" << fd;
            write(fd, &t, sizeof(T));
        }
    }

    /**
     * Method is called to indiciate nothing else will be produced.
     */
    void end()
    {
        if (ok) {
            close(fd);
        }
    }

private:
    bool ok;
    int fd;

};

/*
 * An abstraction for reading `typename T` to a unix pipe. For primitives, this
 * implementation works. For more complex types a concrete implementation
 * should be provided.
 */
template <typename T>
class Consumer : public QThread
{

public:
    explicit Consumer(QObject *parent = nullptr)
        : QThread(parent), ok(false)
    {

    }

    void from(int fd)
    {
        ok = true;
        this->fd = fd;
    }

    ~Consumer()
    {
        close(fd);
    }

    bool consume(T *t)
    {
        qDebug() << "consume" << fd;
        return read(fd, t, sizeof(T)) > 0;
    }

private:
    bool ok;
    int fd;

};

template <typename T>
class Pipeline
{

public:
    // I'm not a fan of having the producer and consumer inside the pipeline?
    // Should those be external and this class should provide methods for
    // writing and reading?
    explicit Pipeline(Producer<T> &initial, Consumer<T> &final)
        : initial(initial), final(final)
    {
        int fd[2];
        if (pipe(fd) == -1) {
            perror("Failed to create pipe");
        }

        initial.to(fd[1]);
        final.from(fd[0]);
    }

    void run()
    {
        initial.start();
        final.start();

        initial.wait();
        final.wait();
    }

private:
    Producer<T> &initial;
    Consumer<T> &final;

};

/**
 * A thread which writes `n` random integers to `fd`.
 */
class RandomUIntProducer : public Producer<int>
{

public:
    explicit RandomUIntProducer(unsigned int n, QObject *parent = nullptr)
        : Producer(parent), n(n)
    {

    }

    void run() override
    {
        for (int i = 0; i < n; ++i) {
            produce(QRandomGenerator::global()->bounded(10));
        }

        end();
    }

private:
    const unsigned int n;

};

template <typename T>
class Logger : public Consumer<T>
{
public:
    explicit Logger(QObject *parent = nullptr)
        : Consumer<T>(parent)
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

int main(void)
{
    RandomUIntProducer producer(10);
    Logger<int> consumer;
    Pipeline<int> pipeline(producer, consumer);
    pipeline.run();
    return 0;
}
