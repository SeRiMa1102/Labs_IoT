#include "console.h"
#include <QScrollBar>
#include <QDebug>
#include <iomanip>
#include <bitset>
#include <string>

Console::Console(QWidget *parent) :
    QPlainTextEdit(parent)
{
    document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::white);
    p.setColor(QPalette::Text, Qt::blue);
    setPalette(p);

}

void Console::putData(const QByteArray &data)
{
    if (type_==0){
        insertPlainText(data + '\n');


    } else if (type_==1){

        std::stringstream stream;
        for (const auto& i : data){

            stream << std::bitset< 8 >(uint(i) ).to_string() << "\t";

        }
        insertPlainText(QString::fromStdString(stream.str()));

    } else if (type_ ==2){
        std::stringstream stream;
        for (const auto& i : data){
            //stream << std::hex << int(i) << "\t";
            //std::stringstream stream0;
            stream << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)i) << "\t";
            //stream0 << int(i) << "\t";
            //qDebug() << QString::fromStdString(stream0.str());

        }
        insertPlainText(QString::fromStdString(stream.str()));
    }

    my_data.append(data);


}

void Console::clear_mine(){
    clear();
    my_data.clear();
}

void Console::representText(int type){

    type_ = type;

    if (type==0){
        clear();
        std::stringstream stream;
        for (const auto& i : my_data){
            stream << i;

        }
        setPlainText(QString::fromStdString(stream.str()));

        QTextCursor tc = this->textCursor();
        tc.setPosition(this->document()->characterCount()-1);
        this->setTextCursor(tc);
        return;
    }


    if (type==1){
        clear();
        std::stringstream stream;
        for (const auto& i : my_data){

            stream << std::bitset< 8 >( uint(i) ).to_string() << "\t";

        }
        setPlainText(QString::fromStdString(stream.str()));

        QTextCursor tc = this->textCursor();
        tc.setPosition(this->document()->characterCount()-1);
        this->setTextCursor(tc);
        return;
    }

    if (type==2){
        clear();
        std::stringstream stream;
        for (const auto& i : my_data){
            stream << std::setfill('0') << std::setw(2) << std::hex << (0xff & (unsigned int)i) << "\t";

        }
        setPlainText(QString::fromStdString(stream.str()));

        QTextCursor tc = this->textCursor();
        tc.setPosition(this->document()->characterCount()-1);
        this->setTextCursor(tc);
        return;
    }

}


void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        break;
    default:
        my_data.append(e->text().toLocal8Bit());
        if (type_ == 0){
            insertPlainText(e->text().toLocal8Bit());
        } else if (type_ == 1){

            std::stringstream stream;
            stream << std::bitset< 8 >(int(my_data.back())).to_string() << "\t";
            insertPlainText(QString::fromStdString(stream.str()));
        } else if (type_ == 2){
            std::stringstream stream;
            stream << std::hex << int(my_data.back()) << "\t";

            insertPlainText(QString::fromStdString(stream.str()));
        }
        emit getData(e->text().toLocal8Bit());
    }
}

void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}
