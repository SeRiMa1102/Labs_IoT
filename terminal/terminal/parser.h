#ifndef PARSER_H
#define PARSER_H
#include <QByteArray>
#include <QObject>
class Parser: public QObject
{
    Q_OBJECT
public:
    Parser();

    virtual ~Parser() {};

    struct bits{
        bool StartW = false;
        bool LengthW = false;
        bool DataW = false;
        bool Crc8W = false;
    };

    bool checkCrc8();

    void parseInput();

    void clearAll();

    void addToBuffer(const QByteArray& input);

public slots:

    void input_pars(const QByteArray& input);

signals:
    void putting_Data(const QByteArray& input);
    void emitting_error();

private:
    QByteArray buffer = {};
    QByteArray Data = {};
    uint8_t Length = 0;
    bits information_bits;
    uint pos = 0;
};



//     ...... 0x23 __0x23___Crc8?__ 0x23
// до 0x23 всё удаляем
// проходим до Crc8 , если не совпадает, то удаляем все до следующего 0x23, иначе удаляем до crc
#endif // PARSER_H
