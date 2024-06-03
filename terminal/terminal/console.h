#ifndef CONSOLE_H
#define CONSOLE_H
//http://doc.crossplatform.ru/qt/4.4.3/threads-mandelbrot.html
#include <QPlainTextEdit>

class Console : public QPlainTextEdit
{
    Q_OBJECT

signals:
    void getData(const QByteArray &data);

public slots:
    void representText(int type);
    void clear_mine();

public:
    explicit Console(QWidget *parent = nullptr);

    void putData(const QByteArray &data);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;

private:
    QByteArray my_data;
    int type_ = 0;

};

#endif // CONSOLE_H
