CREATE TABLE homes (
    id_homes INTEGER PRIMARY  KEY AUTOINCREMENT NOT NULL,
    name   VARCHAR(35) UNIQUE
);

CREATE TABLE organiz (
    id_organiz INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    name   VARCHAR(35) UNIQUE,
    bank   VARCHAR(35),
    sett_account   VARCHAR(35) UNIQUE
);

CREATE TABLE apartament (
    id_apartament INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    id_homes INTEGER,
    id_organiz INTEGER,
    number INTEGER,
    personal_account INTEGER DEFAULT '0',
    total_area DECIMAL(6,2) DEFAULT '0.0',
    inhabed_area DECIMAL(6,2) DEFAULT '0.0',
    balkon DECIMAL(6,2) DEFAULT '0.0',
    loggia DECIMAL(6,2) DEFAULT '0.0',
    surname   VARCHAR(15) DEFAULT '',
    name   VARCHAR(15) DEFAULT '',
    patronymic   VARCHAR(15)  DEFAULT '',
    FOREIGN KEY(id_organiz) REFERENCES organiz(id_organiz) ON DELETE RESTRICT,
    FOREIGN KEY(id_homes) REFERENCES homes(id_homes) ON DELETE RESTRICT
);

CREATE TABLE bay (
    id_apartament INTEGER NOT NULL,
    year_bay INTEGER NOT NULL,
    month_bay INTEGER NOT NULL,
    day_bay INTEGER NOT NULL,
    bay REAL,
    UNIQUE (id_apartament, year_bay, month_bay, day_bay)
);

CREATE TABLE tarif (
            id_tarif INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            year_t INTEGER,
            month_t INTEGER,
            id_usluga INTEGER,
            tarif DECIMAL(6,2) DEFAULT 0.0,
            tarif2 DECIMAL(6,2) DEFAULT 0.0,
            norm DECIMAL(6,2) DEFAULT 0.0,
            UNIQUE (year_t,month_t,id_usluga)
);

CREATE TABLE list_app_usluga (
            id_list_app_usluga INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            id_apartament INTEGER,
            id_usluga INTEGER,
            UNIQUE (id_apartament, id_usluga)
);

CREATE TABLE type_usluga (
            id_type_usluga INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            name VARCHAR(20)
);

CREATE TABLE usluga (
            id_usluga INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            name   VARCHAR(40),
            type_usluga  INTEGER
);

CREATE TABLE pokazanie (
            id_pokazanie INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            id_list_app_usluga INTEGER,
            date_month INTEGER,
            date_year INTEGER,
            pokazanie_home INTEGER,
            pokazanie_end INTEGER,
            UNIQUE (id_list_app_usluga, date_month, date_year)
);

CREATE TABLE debt (
            id_debt INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            year_debt INTEGER,
            month_debt INTEGER,
            id_apartament INTEGER,
            debt REAL,
            UNIQUE (year_debt,month_debt,id_apartament)
);

CREATE TABLE credited (
            id_credited INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            id_list_app_usluga INTEGER,
            month INTEGER,
            year INTEGER,
            credited_of_service REAL
);

CREATE TABLE credited_of_apartament (
            id_credited_of_apartament INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            id_apartament INTEGER NOT NULL,
            month INTEGER NOT NULL,
            year INTEGER NOT NULL,
            credited_with_counter REAL DEFAULT 0.0,
            credited_out_counter REAL DEFAULT 0.0,
            UNIQUE (id_apartament,month,year)
);

CREATE TABLE version (
            version VARCHAR(20)
);

CREATE TABLE men_in_apartament (
            id_apartament INTEGER NOT NULL,
            real_men INTEGER DEFAULT '0',
            rent_men INTEGER DEFAULT '0',
            reserv_men INTEGER DEFAULT '0',
            month INTEGER,
            year INTEGER,
            UNIQUE (id_apartament,month,year),
            FOREIGN KEY(id_apartament) REFERENCES apartament(id_apartament) ON DELETE RESTRICT
);

CREATE TABLE pensioner_living_alone (
            id_apartament INTEGER NOT NULL,
            UNIQUE (id_apartament)
            );

INSERT INTO version (version) VALUES ('1.5');

INSERT INTO type_usluga (name) VALUES ("Счётчики");
INSERT INTO type_usluga (name) VALUES ("На квадратный метр");
INSERT INTO type_usluga (name) VALUES ("На человека");
INSERT INTO type_usluga (name) VALUES ("На квартиру");

INSERT INTO usluga (name, type_usluga )VALUES("Холодная вода",1);
INSERT INTO usluga (name, type_usluga )VALUES("ГВС",1);
INSERT INTO usluga (name, type_usluga )VALUES("Канализация",1);
INSERT INTO usluga (name, type_usluga )VALUES("Электричество",1);
INSERT INTO usluga (name, type_usluga )VALUES("Электричество день",1);
INSERT INTO usluga (name, type_usluga )VALUES("Электричество ночь",1);
INSERT INTO usluga (name, type_usluga )VALUES("Вывоз ТБО",3);
INSERT INTO usluga (name, type_usluga )VALUES("Обработка квитанций",4);
INSERT INTO usluga (name, type_usluga )VALUES("Отопление",2);
INSERT INTO usluga (name, type_usluga )VALUES("Паспортный режим",4);
INSERT INTO usluga (name, type_usluga )VALUES("Тех. содержание и текущий ремонт",2);





