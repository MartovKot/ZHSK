ALTER TABLE list_app_usluga RENAME TO list_app_usluga_temp;

CREATE TABLE list_app_usluga (
    id_list_app_usluga INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    id_apartament INTEGER,
    id_usluga INTEGER,
    used INTEGER DEFAULT '1',
    UNIQUE (id_apartament, id_usluga)
    FOREIGN KEY(id_apartament) REFERENCES apartament(id_apartament) ON DELETE RESTRICT
    FOREIGN KEY(id_usluga) REFERENCES usluga(id_usluga) ON DELETE RESTRICT
);

REPLACE INTO list_app_usluga(id_list_app_usluga, id_apartament,id_usluga)
SELECT id_list_app_usluga, id_apartament,id_usluga
FROM list_app_usluga_temp ;

DROP TABLE list_app_usluga_temp;

UPDATE version SET version = '01.07.003';
