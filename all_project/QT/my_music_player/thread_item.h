#ifndef THREAD_ITEM_H
#define THREAD_ITEM_H

#include <QObject>

class Thread_item : public QObject
{
    Q_OBJECT
public:
    explicit Thread_item(QObject *parent = nullptr);

    void item();
signals:
    void item_startings();

};

#endif // THREAD_ITEM_H
