ALTER TABLE organiz RENAME TO organiz_temp2;

CREATE TABLE organiz (
    id_organiz INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    name   VARCHAR(35) UNIQUE,
    bank   VARCHAR(35),
    sett_account   VARCHAR(35) UNIQUE,
    inn VARCHAR UNIQUE
);

REPLACE INTO organiz(id_organiz, name, bank, sett_account)
SELECT id_organiz, name, bank, sett_account
FROM organiz_temp2 ;

DROP TABLE organiz_temp2;

UPDATE version SET version = '01.07.001';
