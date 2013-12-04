CREATE TABLE homes (
    id_homes INTEGER PRIMARY  KEY AUTOINCREMENT NOT NULL,
    name   VARCHAR(35) UNIQUE
    );


    if (!query.exec(str)) {
        qDebug() << "Unable to create a table homes" << query.lastError();
        LogOut.logout("Unable to create a table homes" + query.lastError().text());
    }

    str  = "CREATE TABLE organiz ( "
                         "id_organiz INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                         "name   VARCHAR(35) UNIQUE, "
                         "bank   VARCHAR(35), "
                         "sett_account   VARCHAR(35) UNIQUE"
                     ");";
    if (!query.exec(str)) {
        qDebug() << "Unable to create a table organiz" << query.lastError();
        LogOut.logout(query.lastError().text());
    }

    str  = "CREATE TABLE apartament ( "
            "id_apartament INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
            "id_homes INTEGER, "
            "id_organiz INTEGER, "
            "number INTEGER, "
            "personal_account INTEGER DEFAULT '0', "
            "total_area DECIMAL(6,2) DEFAULT '0.0', "
            "inhabed_area DECIMAL(6,2) DEFAULT '0.0', "
            "balkon DECIMAL(6,2) DEFAULT '0.0', "
            "loggia DECIMAL(6,2) DEFAULT '0.0', "
            "surname   VARCHAR(15) DEFAULT '', "
            "name   VARCHAR(15) DEFAULT '', "
            "patronymic   VARCHAR(15)  DEFAULT '', "
            "FOREIGN KEY(id_organiz) REFERENCES organiz(id_organiz) ON DELETE RESTRICT, "
            "FOREIGN KEY(id_homes) REFERENCES homes(id_homes) ON DELETE RESTRICT "
            ");";
    if (!query.exec(str)) {
        qDebug() << "Unable to create a table apartament" << query.lastError();
        LogOut.logout(query.lastError().text());
    }

    str  = "CREATE TABLE bay ( "
                         "id_apartament INTEGER NOT NULL,"
                         "year_bay INTEGER NOT NULL,"
                         "month_bay INTEGER NOT NULL,"
                         "day_bay INTEGER NOT NULL,"
                         "bay REAL, "
                         "UNIQUE (id_apartament, year_bay, month_bay, day_bay)"
                     ");" ;
    if (!query.exec(str)) {
        qDebug() << "Unable to create a table bay: " << query.lastError();
        LogOut.logout(query.lastError().text());
    }

    str  = "CREATE TABLE tarif ( "
                        "id_tarif INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                        "year_t INTEGER, "
                        "month_t INTEGER, "
                        "id_usluga INTEGER, "
                        "tarif DECIMAL(6,2) DEFAULT 0.0,"
                        "tarif2 DECIMAL(6,2) DEFAULT 0.0,"
                        "norm DECIMAL(6,2) DEFAULT 0.0,"
                         "UNIQUE (year_t,month_t,id_usluga)"
                     ");";
    if (!query.exec(str)) {
        qDebug() << "Unable to create a table tarif" << query.lastError();
        LogOut.logout(query.lastError().text());
    }

    str  = "CREATE TABLE list_app_usluga ( "
                            "id_list_app_usluga INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                            "id_apartament INTEGER, "
                            "id_usluga INTEGER, "
                            "UNIQUE (id_apartament, id_usluga)"
                     ");";

    if (!query.exec(str)) {
        qDebug() << "Unable to create a table counter" << query.lastError();
        LogOut.logout(query.lastError().text());
    }
    str  = "CREATE TABLE type_usluga ( "
                         "id_type_usluga INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                         "name VARCHAR(20) "
                     ");";
    if (!query.exec(str)) {
        qDebug() << "Unable to create a table type_usluga: " << query.lastError();
        LogOut.logout(query.lastError().text());
    }
    str  = "CREATE TABLE usluga ( "
                         "id_usluga INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                         "name   VARCHAR(40), "
                         "type_usluga  INTEGER "
                     ");";
    if (!query.exec(str)) {
        qDebug() << "Unable to create a table usluga"<< query.lastError();
        LogOut.logout(query.lastError().text());
    }

    str  = "CREATE TABLE pokazanie ( "
                         "id_pokazanie INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                         "id_list_app_usluga INTEGER, "
                         "date_month INTEGER,"
                         "date_year INTEGER,"
                         "pokazanie_home INTEGER, "
                         "pokazanie_end INTEGER, "
                         "UNIQUE (id_list_app_usluga, date_month, date_year)"
                     ");";

    if (!query.exec(str)) {
        qDebug() << "Unable to create a table  pokazanie:  "  << query.lastError();
        LogOut.logout(query.lastError().text());
    }

    str  = "CREATE TABLE debt ( "
                        "id_debt INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                        "year_debt INTEGER, "
                        "month_debt INTEGER,"
                        "id_apartament INTEGER,"
                        "debt REAL, "
                        "UNIQUE (year_debt,month_debt,id_apartament)"
                     ");";

    if (!query.exec(str)) {
        qDebug() << "Unable to create a table  debt:  "  << query.lastError();
        LogOut.logout(query.lastError().text());
    }

    str  = "CREATE TABLE credited ( "
                            "id_credited INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                            "id_list_app_usluga INTEGER, "
                            "month INTEGER,"
                            "year INTEGER,"
                            "credited_of_service REAL "
                     ");";

    if (!query.exec(str)) {
        qDebug() << "Unable to create a table  credited:  "  << query.lastError();
        LogOut.logout(query.lastError().text());
    }

    str  = "CREATE TABLE credited_of_apartament ( "
                            "id_credited_of_apartament INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
                            "id_apartament INTEGER NOT NULL, "
                            "month INTEGER NOT NULL,"
                            "year INTEGER NOT NULL,"
                            "credited_with_counter REAL DEFAULT 0.0, "
                            "credited_out_counter REAL DEFAULT 0.0, "
                            "UNIQUE (id_apartament,month,year)"
                     ");";

    if (!query.exec(str)) {
        qDebug() << "Unable to create a table  credited:  "  << query.lastError();
        LogOut.logout(query.lastError().text());
    }

    str  = "CREATE TABLE version ( "
                         "version VARCHAR(20) "
                     ");";
    if (!query.exec(str)) {
        qDebug() << "Unable to create a table version" << query.lastError();
        LogOut.logout(query.lastError().text());
    }

    str = "CREATE TABLE men_in_apartament ("
            "id_apartament INTEGER NOT NULL,"
            "real_men INTEGER DEFAULT '0', "
            "rent_men INTEGER DEFAULT '0', "
            "reserv_men INTEGER DEFAULT '0', "
            "month INTEGER, "
            "year INTEGER, "
            "UNIQUE (id_apartament,month,year), "
            "FOREIGN KEY(id_apartament) REFERENCES apartament(id_apartament) ON DELETE RESTRICT"
            ");";
    if (!query.exec(str)) {
        qDebug() << "Unable to create a table men_in_apartament" << query.lastError();
        LogOut.logout(query.lastError().text());
    }

//------------------------------
    if (!query.exec("INSERT INTO version (version) VALUES ('1.4');")) {
        qDebug() << "Unable to insert a table version" << query.lastError();
        LogOut.logout(query.lastError().text());
    }

    QStringList strL;
    strL << QObject::trUtf8("Счётчики")
         << QObject::trUtf8("На квадратный метр")
         << QObject::trUtf8("На человека")
         << QObject::trUtf8("На квартиру");
    for (int i=1;i<=4;i++)
    {
        str  = "INSERT INTO type_usluga (name) VALUES ( '"+strL[i-1]+"');";
        if (!query.exec(str))
                {
                    qDebug() << "Unable to insert a value of type_usluga" << i << query.lastError();
                    LogOut.logout(query.lastError().text());
                }
    }
    strL.clear();
    strL    << QObject::trUtf8("Холодная вода")
            << QObject::trUtf8("ГВС")
            << QObject::trUtf8("Канализация")
            << QObject::trUtf8("Электричество")
            << QObject::trUtf8("Электричество день")
            << QObject::trUtf8("Электричество ночь")
            << QObject::trUtf8("Вывоз ТБО")
            << QObject::trUtf8("Обработка квитанций")
            << QObject::trUtf8("Отопление")
            << QObject::trUtf8("Паспортный режим")
            << QObject::trUtf8("Тех. содержание и текущий ремонт");
    int Arr[] = {1,1,1,1,1,1,3,4,2,4,2};
    for (int i=1;i<=int((sizeof(Arr)/sizeof(Arr[0])));i++)
    {
        str  = "INSERT INTO usluga (name, type_usluga )VALUES ('"+strL[i-1]+"','%1');";

        if (!query.exec(str.arg(Arr[i-1])))
                {
                    qDebug() << "Unable to insert a value of usluga" << str << query.lastError();
                    LogOut.logout(query.lastError().text());
                }
    }
