CREATE TABLE men_in_apartament_t (
            id_apartament INTEGER NOT NULL,
            real_men INTEGER DEFAULT '0',
            rent_men INTEGER DEFAULT '0',
            reserv_men INTEGER DEFAULT '0',
            month TEXT,
            year INTEGER,
            UNIQUE (id_apartament,month,year),
            FOREIGN KEY(id_apartament) REFERENCES apartament(id_apartament) ON DELETE RESTRICT
);

REPLACE INTO men_in_apartament_t (id_apartament, real_men, rent_men, reserv_men, month, year)
SELECT id_apartament, real_men, rent_men, reserv_men, month, year
FROM  men_in_apartament WHERE  men_in_apartament.month>=10;

REPLACE INTO men_in_apartament_t (id_apartament, real_men, rent_men, reserv_men, month, year)
SELECT id_apartament, real_men, rent_men, reserv_men, '0'||month, year
FROM  men_in_apartament WHERE  men_in_apartament.month<10;

DROP TABLE men_in_apartament;

CREATE TABLE men_in_apartament (
            id_apartament INTEGER NOT NULL,
            real_men INTEGER DEFAULT '0',
            rent_men INTEGER DEFAULT '0',
            reserv_men INTEGER DEFAULT '0',
            date_men_in_apartament INTEGER NOT NULL,
            UNIQUE (id_apartament,date_men_in_apartament),
            FOREIGN KEY(id_apartament) REFERENCES apartament(id_apartament) ON DELETE RESTRICT
);

REPLACE INTO men_in_apartament (id_apartament, real_men, rent_men, reserv_men, date_men_in_apartament)
SELECT id_apartament, real_men, rent_men, reserv_men, STRFTIME('%s',t.year||'-'||t.month||'-01')
FROM  men_in_apartament_t t;

DROP TABLE men_in_apartament_t;

UPDATE version SET version = '01.06.007';
