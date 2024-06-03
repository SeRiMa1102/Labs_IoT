#include "parser.h"
#include <QDebug>

Parser::Parser()
{

}

void Parser::clearAll(){
    information_bits.Crc8W = false;
    information_bits.DataW =false;
    information_bits.LengthW = false;
    information_bits.StartW = false;

    Data.clear();
    Length = 0;
    //cccccc
    pos = 0;
}

void Parser::addToBuffer(const QByteArray& input){
    buffer.append(input);
}

bool Parser::checkCrc8(){

    char crc = 0xFF;
    int i;

    int len = Data.length();
    int g = 0;
    while (len--)
    {
        crc ^= Data[g]&0xff;

        g++;

        for (i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }

//    qDebug() << '\n'<< "Crc my " << uint8_t(buffer.at(pos+2)) << " Crc calculated " << uint8_t(crc)  << '\n';
    return crc == buffer.at(pos+2);

}

void Parser::input_pars(const QByteArray& input){
    buffer.append(input);
    while (!buffer.isEmpty()){

//        qDebug() << '\n'<<"Pos 0  " << uint8_t(buffer.at(0))<<'\n';
        if (information_bits.StartW == false){

            if (uint8_t(buffer[0]) != 0x23){
//                qDebug() << '\n'<<"Pos not 23 " << uint8_t(buffer.at(0))<<'\n';
                buffer.remove(0,1);
            } else {
//              qDebug() << '\n'<<"Pos is 23" << uint8_t(buffer.at(0))<< '\n';
                information_bits.StartW = true;

            }

        } else if (buffer.size() < 2) { return;
        } else if (information_bits.LengthW == false){
            if (uint8_t(buffer.at(1)) == 0){
//                qDebug() << '\n'<<"len 0 " << uint8_t(buffer.at(1)) << '\n';
                clearAll();
                buffer.remove(0,1);

            } else {
//                qDebug() << '\n'<<"len > 0 " << uint8_t(buffer.at(1))<< '\n';
                Length = uint8_t(buffer.at(1));
                information_bits.LengthW = true;
            }
        } else if (buffer.size() < pos+3) { return;
        } else if (information_bits.DataW == false){
            if (pos < uint(Length) - 1){
//                qDebug() << '\n'<<"byte number "<< pos  << "  " << uint8_t(buffer.at(pos+2))<< '\n';
                Data.append(uint8_t(buffer.at(pos+2)));
                pos++;
            } else if (pos == int(Length) - 1){
                Data.append(uint8_t(buffer.at(pos+2)));
//                qDebug() << '\n'<<"last byte "<< pos  << "  " << uint8_t(buffer.at(pos+2)) << '\n';
                pos++;
                information_bits.DataW = true;
            }
        } else if (buffer.size() < int(Length)+3) { return;
        } else if (information_bits.Crc8W == false) {
            if (checkCrc8()){
//                qDebug() << '\n'<<" Complete" << '\n';
                emit putting_Data(Data);
                buffer.remove(0,uint(Length)+3);
                clearAll();
            } else {
//                qDebug() << '\n'<< "Crc not !!!"  << '\n';
                clearAll();
                buffer.remove(0,1);
            }
        }

    }
}

void Parser::parseInput(){

}


//     ...... 0x23 __0x23___Crc8?__ 0x23
// до 0x23 всё удаляем
// проходим до Crc8 , если не совпадает, то удаляем все до следующего 0x23, иначе удаляем до crc включительно
