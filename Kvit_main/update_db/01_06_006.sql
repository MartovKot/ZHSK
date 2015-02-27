CREATE TABLE credited_of_apartament_t (
            id_credited_of_apartament INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            id_apartament INTEGER NOT NULL,
            month TEXT NOT NULL,
            year INTEGER NOT NULL,
            credited_with_counter REAL DEFAULT 0.0,
            credited_out_counter REAL DEFAULT 0.0,
            UNIQUE (id_apartament,month,year)
);

REPLACE INTO credited_of_apartament_t(id_credited_of_apartament, id_apartament, month, year, credited_with_counter, credited_out_counter )
SELECT id_credited_of_apartament, id_apartament, month, year, credited_with_counter, credited_out_counter
FROM credited_of_apartament WHERE credited_of_apartament.month>=10;

REPLACE INTO credited_of_apartament_t(id_credited_of_apartament, id_apartament, month, year, credited_with_counter, credited_out_counter )
SELECT id_credited_of_apartament, id_apartament, '0'||month, year, credited_with_counter, credited_out_counter
FROM credited_of_apartament WHERE credited_of_apartament.month<10;

DROP TABLE credited_of_apartament;

CREATE TABLE credited_of_apartament (
            id_credited_of_apartament INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            id_apartament INTEGER NOT NULL,
            date_credited_of_apartament INTEGER NOT NULL,
            credited_with_counter REAL DEFAULT 0.0,
            credited_out_counter REAL DEFAULT 0.0,
            UNIQUE (id_apartament,date_credited_of_apartament)
);

REPLACE INTO credited_of_apartament(id_credited_of_apartament, id_apartament,date_credited_of_apartament, credited_with_counter, credited_out_counter )
SELECT id_credited_of_apartament, id_apartament, STRFTIME('%s',t.year||'-'||t.month||'-01'), credited_with_counter, credited_out_counter
FROM credited_of_apartament_t t;

DROP TABLE credited_of_apartament_t;

UPDATE version SET version = '01.06.006';
