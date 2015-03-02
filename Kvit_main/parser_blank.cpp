#include "parser_blank.h"
#include "progressdialog.h"

parser_blank::parser_blank(QWidget *parent):
    QWidget(parent)
{

}
void parser_blank::creat_blank(QString name,const Apartment & apartment, QDate date)
{
    QFile f_m,f_s,f_u;
    f_m.setFileName("./blank/blank_main.html");
    f_s.setFileName("./blank/blank_s.html");
    f_u.setFileName("./blank/blank_u.html");


    f_m.open(QIODevice::ReadOnly);//открываем основной файл-бланк
    QTextStream in_m(&f_m);
    QString str_m = in_m.readAll();
    f_m.close();

    f_s.open(QIODevice::ReadOnly);//открываем файл счётчиков
    QTextStream in_s(&f_s);
    QString str_s = in_s.readAll();
    f_s.close();

    f_u.open(QIODevice::ReadOnly); //открываем файл услуг
    QTextStream in_u(&f_u);
    QString str_u = in_u.readAll();
    f_u.close();

    int find_pos;

    str_m = process_main(str_m,apartment);

    // обработка счётчиков
    find_pos = str_m.indexOf("@tab_sch#");
    while (find_pos>=0){
        str_m.replace(find_pos,9,process_schet(str_s, apartment.getId(), date));
        find_pos = str_m.indexOf("@tab_sch#");
    }

    // обработка услуг
    find_pos = str_m.indexOf("@tab_usl#");
    while (find_pos>=0){
        str_m.replace(find_pos,9,process_usluga(str_u, apartment.getId(), date));
        find_pos = str_m.indexOf("@tab_usl#");
    }

    for(int i=0;i<name.size();i++){
        if(name.at(i)==QChar('"')){
            name.replace(i, 1, " ");
        }
    }
    QString  str_file;                          //имя выходного файла
    str_file = str_folder +"/" +name;


    QFile f_out(str_file);                          //создаём файл
    f_out.open(QIODevice::WriteOnly);

    QTextStream out(&f_out);
    out<<str_m;                                     //записываем в него инфу
    f_out.close();
    /*qDebug()<<*/f_out.copy(str_folder_arhiv + "/" + name); //копируем файл в архив
}

QString parser_blank::process_usluga(QString str_in_usl, int id_app, QDate date)
{
    QString str_out;
    QStringList strlst_find;
    QList<int> ServiceList;
    ServiceList = db.is_ListIdServiceOutCounter(id_app);


    strlst_find << "@Usliga#"<<"@Tarif#"<<"@Nachisl#"<<"@Pererasch#"<<"@ItogUsluga#";
    str_out = "";

    int find_pos;

    for (int i=0;i<ServiceList.count();i++){
        str_out = str_out + str_in_usl;
        find_pos = str_out.indexOf(strlst_find.at(0));
        if(find_pos>=0){
            str_out.replace(find_pos,strlst_find.at(0).size(), db.is_NameService(ServiceList.at(i)));
        }
        find_pos = str_out.indexOf(strlst_find.at(1));
        if(find_pos>=0){
            str_out.replace(find_pos,strlst_find.at(1).size(),
                            QString::number(tbl_tariff.is_Tariff(ServiceList.at(i),date)));
        }
        find_pos = str_out.indexOf(strlst_find.at(2));
        if(find_pos>=0){
            str_out.replace(find_pos,strlst_find.at(2).size(),
                            QString::number(db.CreditedForReport(id_app,ServiceList.at(i),date)));
        }
        find_pos = str_out.indexOf(strlst_find.at(3));
        if(find_pos>=0){
            str_out.replace(find_pos,strlst_find.at(3).size(), "0");
        }
        find_pos = str_out.indexOf(strlst_find.at(4));
        if(find_pos>=0){
            str_out.replace(find_pos,strlst_find.at(4).size(),
                            QString::number(db.CreditedForReport(id_app,ServiceList.at(i),date)));
        }
    }
    return str_out;
}
QString parser_blank::process_schet(QString str_in_sch,const int id_app, QDate date)
{
    QString str_out="";
    QStringList strlst_find;
    QList<int> CounterList;

    CounterList = db.is_ListIdServiceWithCounter(id_app);
    strlst_find<<"@Schetchik#"<<"@PokazanieEnd#"<<"@Tarif#"<<"@Tarif2#";

    int find_pos;
    for(int i=0;i<CounterList.size();i++){
        str_out = str_out + str_in_sch;
        find_pos = str_out.indexOf(strlst_find.at(0));
        if(find_pos>=0){ //Название Счётчика
            str_out.replace(find_pos,strlst_find.at(0).size(), db.is_NameService(CounterList.at(i)));
        }
        find_pos = str_out.indexOf(strlst_find.at(1));
        if(find_pos>=0){ //Показание
            str_out.replace(find_pos,strlst_find.at(1).size(),
                            QString::number(db.is_Pokazanie(
                                                db.is_idListAppUsluga(id_app,CounterList.at(i)),date)));
        }
        find_pos = str_out.indexOf(strlst_find.at(2));
        if(find_pos>=0){ //тариф
            str_out.replace(find_pos,strlst_find.at(2).size(),
                            QString::number(tbl_tariff.is_Tariff(CounterList.at(i),date)));
        }
        find_pos = str_out.indexOf(strlst_find.at(3));
        if(find_pos>=0){ //тариф2
            double t2 = tbl_tariff.is_Tariff(CounterList.at(i),date,2);
            if (t2!=0.0 ){
                str_out.replace(find_pos,strlst_find.at(3).size(), " / "+QString::number(t2));
            }else{
                str_out.replace(find_pos,strlst_find.at(3).size(), "");
            }
        }

    }
    return str_out;
}

void parser_blank::generating()
{

    str_folder_arhiv = "./arhiv/"+QDateTime::currentDateTime().toString("yyMMdd-hhmmss");   //архивный каталог
    str_folder = "./kvit/"+QDate::currentDate().toString("MMMM_yyyy");                      // текущтй каталог
    QDir dir;
    dir.setPath("./kvit/");
    if(!dir.isReadable()){
        dir.setPath("./");
        dir.mkdir("./kvit");
    }
    dir.setPath(str_folder);
    if(!dir.isReadable()){
        dir.setPath("./");
        dir.mkdir(str_folder);//создали новый каталог
    }else{
        QMessageBox::warning(this,trUtf8("Предупреждение"),
                             QObject::trUtf8("Формирование уже производилось"),QMessageBox::Ok);
//        return;
    }
    dir.setPath("./arhiv/");
    if(!dir.isReadable()){
        dir.setPath("./");
        dir.mkdir("./arhiv");
    }
    dir.setPath(str_folder_arhiv);
    if(!dir.isReadable()){
        dir.setPath("./");
        dir.mkdir(str_folder_arhiv);//создали архивный каталог
    }
    QString str_NameFile_base; //общая часть названия файлов
    QString str_NameFile_pach; //отличительная часть файлов

    QStringList str_L = db.sum_app(ConfData.Org_id,ConfData.Home_id);

    Organization organization;
    organization.setId(ConfData.Org_id);
    Home home;
    home.setId(ConfData.Home_id);
    str_NameFile_base = organization.getName() + " " + home.getName();


    QProgressDialog progress(trUtf8("Создание файлов..."), trUtf8("Отмена"), 0, str_L.size()-1, this);
//    ProgressDialog progress;
    progress.setWindowModality(Qt::WindowModal);
    progress.setWindowTitle(trUtf8("Расчёт"));

//    progress.run();
    progress.show();
    for(int i=0;i<str_L.size();i++){
        progress.setValue(i);
        if (progress.wasCanceled())
            break;
        str_NameFile_pach = QObject::trUtf8(" кв ") + str_L.at(i);
        Apartment apartment(ConfData.Home_id,ConfData.Org_id,str_L.at(i).toInt());
        creat_blank(str_NameFile_base+str_NameFile_pach+".html",
                    apartment,
                    ConfData.date);
    }
    progress.setValue(str_L.size()-1);
}
void parser_blank::setDir(QString str_dirFolder)
{
    str_folder = "./kvit/"+str_dirFolder;
}

QStringList parser_blank::OutStrL()
{
    QDir dir(str_folder);
    QStringList strL,outfile;
    QFile file;

    strL = dir.entryList(QDir::Files,QDir::Name);
    for(int i = 0;i<strL.size();i++){
        file.setFileName(str_folder+"/"+strL.at(i));
        file.open(QIODevice::ReadOnly);
        QTextStream in_u(&file);
        outfile.append(in_u.readAll());
        file.close();
    }
    return outfile;
}

QString parser_blank::isfirstfile()
{
    QDir dir(str_folder);
    QStringList strL;
    strL = dir.entryList(QDir::Files,QDir::Name);
    return str_folder+"/"+strL.first();
}
QString parser_blank::isnextfile()
{
    QDir dir(str_folder);
    QStringList strL;
    strL = dir.entryList(QDir::Files,QDir::Name);
    return str_folder+"/"+strL.at(0);
}

void parser_blank::setDate( int year,int month, int id_home, int id_org)
{
//    ConfData.month = month;
//    ConfData.year = year;
    ConfData.date = QDate(year,month,1);
    //-----------
    ConfData.Home_id = id_home;
    ConfData.Org_id = id_org;
    //-----------
}

QString parser_blank::process_main(QString str_in, const Apartment & apartment)
{
    QString str_out;
    QStringList strL_find, strL_replace;
    int find_pos;
    Organization *organization = new Organization(apartment.getId());
    Home home;
    home.setId(ConfData.Home_id);


    strL_find << "@NameOrganization#"   << "@FIO#"
              << "@LSh#"
              << "@RealMen#"
              << "@RentMen#"
              << "@ReservMen#"
              << "@TotalArea#"
              << "@LivedArea#"
              << "@Balkon#"             << "@Lodjia#"
              << "@Adres#"
              << "@SmallDate#"
              << "@DateOpl#"
              << "@SumOpl#"
              << "@DolgOpl#"
              << "@prKOpl#"
              << "@ItogKOpl#"
              << "@INN#"
              << "@Bank#";

    DateOfUnixFormat u_date(ConfData.date);
    strL_replace << organization->getName()     << apartment.is_FIO_payer()
                 << QString::number(db.is_LSh(apartment.getId()))
                 << QString::number(db.is_RealMen(apartment.getId(),ConfData.date))
                 << QString::number(db.is_RentMen(apartment.getId(),ConfData.date))
                 << QString::number(db.is_ReservMen(apartment.getId(),ConfData.date))
                 << QString::number(apartment.getTotalArea())
                 << QString::number(apartment.getLivedArea())
                 << QString::number(apartment.getBalkon())       << QString::number(apartment.getLodjia())
                 << home.getName()
                    + QObject::trUtf8(" кв. ")
                    + QString::number(apartment.getNumber())
                 << QString::number(ConfData.date.month())
                    + " / " + QString::number(ConfData.date.year())
                 << QDate::longMonthName(ConfData.date.month())
                    + "  " + QString::number(ConfData.date.year()) + QObject::trUtf8(" г.")
                 << QString::number(db.AmountToPay(apartment.getId(),u_date.Second()))
                 << db.is_Debt(apartment.getId(),ConfData.date)
                 << QString::number(db.AmountForServices(apartment.getId(), u_date.Second()))
                 << QString::number(db.AmountForServices(apartment.getId(), u_date.Second()))
                 << QObject::trUtf8(" ИНН ") + organization->getINN()
                 << organization->getBank();

    str_out = str_in;
    for (int i=0; i<strL_find.size(); i++){
        find_pos = str_out.indexOf(strL_find.at(i));
        while (find_pos>=0){
            str_out.replace(find_pos,strL_find.at(i).size(),strL_replace.at(i));
            find_pos = str_out.indexOf(strL_find.at(i));
        }
    }

    delete organization;
    return str_out;
}
