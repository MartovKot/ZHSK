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
    QString  str_file;                           //имя выходного файла
    str_file = str_folder +"/" +name;


    QFile f_out(str_file);                          //создаём файл
    f_out.open(QIODevice::WriteOnly);

    QTextStream out(&f_out);
    out<<str_m;                                     //записываем в него инфу
    f_out.close();
    f_out.copy(str_folder_arhiv + "/" + name); //копируем файл в архив
}

QString parser_blank::process_usluga(QString str_in_usl, int id_app, QDate date)
{
    QString str_out;
    QStringList strlst_find;
    QList<int> ServiceList;
    Apartment apartment(id_app);
    //ServiceList = apartment.getListIdServiceOutCounter();
    ServiceList = apartment.getListIdServiceFull();

    strlst_find << "@Usliga#" << "@Tarif#" << "@Tarif2#" << "@Volume#"
                << "@Unit#" << "@PokazanieCurrent#" << "@PokazanieEnd#" << "@Nachisl#"
                << "@Pererasch#" << "@ItogUsluga#";
    str_out = "";



    for (int i=0;i<ServiceList.count();i++){
        int find_pos;
        Service service(ServiceList.at(i));
        str_out = str_out + str_in_usl;
        find_pos = str_out.indexOf(strlst_find.at(0));
        if(find_pos>=0){  //Наименование
            str_out.replace(find_pos,strlst_find.at(0).size(), service.getName());
        }
        find_pos = str_out.indexOf(strlst_find.at(1));
        if(find_pos>=0){   //Тариф
            str_out.replace(find_pos,strlst_find.at(1).size(),
                            QString::number(tbl_tariff.is_Tariff(ServiceList.at(i),date)));
        }
        find_pos = str_out.indexOf(strlst_find.at(2));
        if(find_pos>=0){ //Тариф2
            double t2 = tbl_tariff.is_Tariff(ServiceList.at(i),date,2);
            if (t2!=0.0 ){
                str_out.replace(find_pos,strlst_find.at(2).size(), " / "+QString::number(t2));
            }else{
                str_out.replace(find_pos,strlst_find.at(2).size(), "");
            }
        }

        find_pos = str_out.indexOf(strlst_find.at(3));
        if(find_pos>=0){  //объём
            str_out.replace(find_pos,strlst_find.at(3).size(),
                            Fast_Calculation::Volume(id_app,ServiceList.at(i),date));
        }
        find_pos = str_out.indexOf(strlst_find.at(4));
        if(find_pos>=0){  //единицы измерения
            str_out.replace(find_pos,strlst_find.at(4).size(),
                            Fast_Calculation::Unit(id_app,ServiceList.at(i),date));
        }

        find_pos = str_out.indexOf(strlst_find.at(5));
        if(find_pos>=0){ //Показание конец месяца
            QString t;
            Indications indications(ServiceList.at(i),apartment.getId(),date.addMonths(1));
            if ( service.getIdType() == 1) {
                t = QString::number(indications.valueIndicationHome());
            }else{
                t = "-";
            }

            str_out.replace(find_pos,strlst_find.at(5).size(), t);
        }

        find_pos = str_out.indexOf(strlst_find.at(6));
        if(find_pos>=0){ //Показание начало месяца
            QString t;
            Indications indications(ServiceList.at(i),apartment.getId(),date.addMonths(0));
            if ( service.getIdType() == 1) {
                t = QString::number(indications.valueIndicationHome());
            }else{
                t = "-";
            }
            str_out.replace(find_pos,strlst_find.at(6).size(), t);
        }
        find_pos = str_out.indexOf(strlst_find.at(7));
        if(find_pos>=0){
            QString t;
            t = Fast_Calculation::CreditedForReport(id_app,ServiceList.at(i),date);
            str_out.replace(find_pos,strlst_find.at(7).size(),t);
        }

        find_pos = str_out.indexOf(strlst_find.at(8));
        if(find_pos>=0){ //Перерасчёт
            str_out.replace(find_pos,strlst_find.at(8).size(), "0");
        }

        find_pos = str_out.indexOf(strlst_find.at(9));
        if(find_pos>=0){
            QString t;
            t = Fast_Calculation::CreditedForReport(id_app,ServiceList.at(i),date);
            str_out.replace(find_pos,strlst_find.at(9).size(),t);
        }
    }
    return str_out;
}
QString parser_blank::process_schet(QString str_in_sch,const int id_apartment, QDate date)
{
    QString str_out="";
    QStringList strlst_find;
    QList<int> CounterList;
    Apartment apartment(id_apartment);
    CounterList = apartment.getListIdServiceWithCounter();

    strlst_find<<"@Schetchik#"<<"@PokazanieEnd#"<<"@Tarif#"<<"@Tarif2#" << "@PokazanieCurrent#";


    for(int i=0;i<CounterList.size();i++){
        int find_pos;
        Service service(CounterList.at(i));
        str_out = str_out + str_in_sch;
        find_pos = str_out.indexOf(strlst_find.at(0));
        if(find_pos>=0){ //Название Счётчика
            str_out.replace(find_pos,strlst_find.at(0).size(), service.getName());
        }
        find_pos = str_out.indexOf(strlst_find.at(1));
        if(find_pos>=0){ //Показание
            Indications indications(CounterList.at(i),apartment.getId(),date.addMonths(1));
            str_out.replace(find_pos,strlst_find.at(1).size(),
                            QString::number(indications.valueIndicationHome()));
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
        
        find_pos = str_out.indexOf(strlst_find.at(4));
        if(find_pos>=0){ //Показание текущие
            Indications indications2(CounterList.at(i),apartment.getId(),date.addMonths(1));
            str_out.replace(find_pos,strlst_find.at(1).size(),
                            QString::number(indications2.valueIndicationHome()));
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
//        QMessageBox::warning(this,trUtf8("Предупреждение"),
//                             QObject::trUtf8("Формирование уже производилось"),QMessageBox::Ok);
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

    Home home(ConfData.Home_id);
    str_NameFile_base = home.organization()->name() + " " + home.getName();


    QProgressDialog progress(
                trUtf8("Создание файлов..."),
                trUtf8("Отмена"),
                0,
                home.apartments().count()-1,
                this);

    progress.setWindowModality(Qt::WindowModal);
    progress.setWindowTitle(trUtf8("Создание файлов..."));

    progress.show();
    for(int i=0;i<home.apartments().count();i++){
        progress.setValue(i);
        if (progress.wasCanceled())
            break;
        str_NameFile_pach = QObject::trUtf8(" кв ") + home.apartments().at(i)->getNumber();
        creat_blank(str_NameFile_base+str_NameFile_pach+".html",
                    *home.apartments().at(i),
                    ConfData.date);
    }
    progress.setValue(home.apartments().count()-1);
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

    Organization organization(apartment.getIdOrganization());
    Home home(ConfData.Home_id);


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
              << "@Bank#"
              << "@DateEndOpl#"
              << "@BIK#"
              << "@DatePokazanie#";

    DateOfUnixFormat u_date(ConfData.date);
    strL_replace << organization.name()     << apartment.is_FIO_payer()
                 << QString::number(apartment.getPersonalAccount())
                 << QString::number(apartment.getRealMen(ConfData.date))
                 << QString::number(apartment.getRentMen(ConfData.date))
                 << QString::number(apartment.getReservMen(ConfData.date))
                 << QString::number(apartment.getTotalArea())
                 << QString::number(apartment.getLivedArea())
                 << QString::number(apartment.getBalkon())       << QString::number(apartment.getLodjia())
                 << home.getName()
                    + QObject::trUtf8(" кв. ")
                    + apartment.getNumber()
                 << QString::number(ConfData.date.month())
                    + " / " + QString::number(ConfData.date.year())
                 << QDate::longMonthName(ConfData.date.month())
                    + "  " + QString::number(ConfData.date.year()) + QObject::trUtf8(" г.")
                 << QString::number(Fast_Calculation::AmountToPay(apartment.getId(),u_date.Second()))
                 << Fast_Calculation::Debt(apartment.getId(),ConfData.date)
                 << QString::number(Fast_Calculation::AmountForServices(apartment.getId(), u_date.Second()))
                 << QString::number(Fast_Calculation::AmountForServices(apartment.getId(), u_date.Second()))
                 << QObject::trUtf8(" ИНН ") + organization.inn()
                 << organization.bank()
                 << "10." + nextMonth(ConfData.date.month()) + "." + QString::number(nextYearN(ConfData.date.year()))
                 << QObject::trUtf8(" БИК ") + organization.bik()
                 << QDate::longMonthName(nextMonthN(ConfData.date.month()))
                    + "  " + QString::number(nextYearN(ConfData.date.year())) + QObject::trUtf8(" г.");
    qDebug()<< "process_main - " <<home.getName()
              + QObject::trUtf8(" кв. ")
              + apartment.getNumber();

    str_out = str_in;
    for (int i=0; i<strL_find.size(); i++){
        int find_pos;
        find_pos = str_out.indexOf(strL_find.at(i));
        while (find_pos>=0){
            str_out.replace(find_pos,strL_find.at(i).size(),strL_replace.at(i));
            find_pos = str_out.indexOf(strL_find.at(i));
        }
    }

    return str_out;
}

QString parser_blank::nextMonth(int month)
{
    QString out = "";
    if (month < 1 || month > 12){
        return out;
    }else {
        month = month + 1;
        if (month == 13) {
            out = "01";
            month = 1;
        }
        if (month < 10){
            out = "0" + QString::number(month);
        }else{
            out = QString::number(month);
        }
    }
    return  out;
}

int parser_blank::nextMonthN(int month) {
    if (month >= 12) {
        return 1;
    }
    return month + 1;
}

int parser_blank::nextYearN(int year) {
    if (ConfData.date.month() == 12) {
        return year + 1;
    }
    return year;
}
