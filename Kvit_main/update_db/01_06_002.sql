ALTER TABLE tarif RENAME TO tarif_2;

CREATE TABLE tarif (
            id_tarif INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            year_t INTEGER,
            month_t TEXT,
            id_usluga INTEGER,
            tarif DECIMAL(6,2) DEFAULT 0.0,
            tarif2 DECIMAL(6,2) DEFAULT 0.0,
            norm DECIMAL(6,2) DEFAULT 0.0,
            UNIQUE (year_t,month_t,id_usluga)
);

REPLACE INTO tarif(id_tarif, year_t, month_t, id_usluga, tarif, tarif2, norm)
SELECT id_tarif, year_t, month_t, id_usluga, tarif, tarif2, norm
FROM tarif_2 WHERE tarif_2.month_t>=10;

REPLACE INTO tarif(id_tarif, year_t, month_t, id_usluga, tarif, tarif2, norm)
SELECT id_tarif, year_t, '0'||month_t, id_usluga, tarif, tarif2, norm
FROM tarif_2 WHERE tarif_2.month_t<10;

CREATE TABLE tariff (
            id_tariff INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            tariff_date INTEGER NOT NULL,
            id_usluga INTEGER NOT NULL,
            tariff NUMERIC DEFAULT 0.0,
            tariff2 NUMERIC DEFAULT 0.0,
            norm NUMERIC DEFAULT 0.0,
            UNIQUE (tariff_date,id_usluga)
);

REPLACE INTO tariff (id_tariff, tariff_date, id_usluga, tariff, tariff2, norm)
SELECT id_tarif, STRFTIME('%s',t.year_t||'-'||t.month_t||'-01'), id_usluga, tarif, tarif2, norm
FROM tarif t;

DROP TABLE tarif;

UPDATE version SET version = '01.06.002';
