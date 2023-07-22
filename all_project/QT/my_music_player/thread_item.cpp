#include "thread_item.h"
#include"widget.h"

Thread_item::Thread_item(QObject *parent) : QObject(parent)
{

}

void Thread_item::item()
{
    //暂停和开始
    if(Widget::item_Pause_start_Button == true)
    {
       qDebug() << "thread_item";
       emit item_startings();
       Widget::item_Pause_start_Button = false;
    }
}
