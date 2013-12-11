CREATE TABLE pokazanie_t1 (
            id_pokazanie INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            id_list_app_usluga INTEGER,
            date_month TEXT,
            date_year INTEGER,
            pokazanie_home INTEGER,
            pokazanie_end INTEGER,
            UNIQUE (id_list_app_usluga, date_month, date_year)
);

REPLACE INTO pokazanie_t1(id_pokazanie, id_list_app_usluga, date_month, date_year, pokazanie_home, pokazanie_end)
SELECT id_pokazanie, id_list_app_usluga, date_month, date_year, pokazanie_home, pokazanie_end
FROM pokazanie WHERE pokazanie.date_month>=10;

REPLACE INTO pokazanie_t1(id_pokazanie, id_list_app_usluga, date_month, date_year, pokazanie_home, pokazanie_end)
SELECT id_pokazanie, id_list_app_usluga, '0'||date_month, date_year, pokazanie_home, pokazanie_end
FROM pokazanie WHERE pokazanie.date_month<10;

DROP TABLE pokazanie;

CREATE TABLE pokazanie (
            id_pokazanie INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            id_list_app_usluga INTEGER,
            date_pokazanie INTEGER,
            pokazanie_home INTEGER,
            pokazanie_end INTEGER,
            UNIQUE (id_list_app_usluga, date_pokazanie)
);

REPLACE INTO pokazanie (id_pokazanie, id_list_app_usluga, date_pokazanie, pokazanie_home, pokazanie_end)
SELECT id_pokazanie, id_list_app_usluga, STRFTIME('%s',t.date_year||'-'||t.date_month||'-01'), pokazanie_home, pokazanie_end
FROM pokazanie_t1 t;

DROP TABLE pokazanie_t1;

UPDATE version SET version = '01.06.003';
