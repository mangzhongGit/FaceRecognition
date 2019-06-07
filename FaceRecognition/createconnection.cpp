#include <tools.h>
// 创建连接数据库
bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    if(QSqlDatabase::contains("face.db"))
    db.setDatabaseName("face.db");
    if(!db.open())
    {
        qDebug()<<"can't open the database face.db";
        return false;
    }
    QSqlQuery query;
    // 创建 人脸身份信息 表
    query.exec("create table face ("
               "id int primary key,"
               "name varchar,"
               "gender varchar,"
               "age int,"
               "date varchar"
               ")");

    return true;
}
