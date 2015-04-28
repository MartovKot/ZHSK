CREATE TABLE credited_t (
            id_credited INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            id_list_app_usluga INTEGER,
            month TEXT,
            year INTEGER,
            credited_of_service REAL
);

REPLACE INTO credited_t(id_credited, id_list_app_usluga, month, year, credited_of_service )
SELECT id_credited, id_list_app_usluga, month, year, credited_of_service
FROM credited WHERE credited.month>=10;

REPLACE INTO credited_t(id_credited, id_list_app_usluga, month, year, credited_of_service )
SELECT id_credited, id_list_app_usluga, '0'||month, year, credited_of_service
FROM credited WHERE credited.month<10;

DROP TABLE credited;

CREATE TABLE credited (
            id_credited INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            id_list_app_usluga INTEGER,
            date_credited INTEGER,
            credited_of_service REAL
);

REPLACE INTO credited(id_credited, id_list_app_usluga, date_credited, credited_of_service )
SELECT id_credited, id_list_app_usluga,STRFTIME('%s',t.year||'-'||t.month||'-01'), credited_of_service
FROM credited_t t;

DROP TABLE credited_t;

UPDATE version SET version = '01.06.005';
