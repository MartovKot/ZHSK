ALTER TABLE debt RENAME TO debt_temp;

CREATE TABLE debt (
            date_debt INTEGER,
            id_apartament INTEGER,
            debt REAL,
            UNIQUE (date_debt,id_apartament)
);

REPLACE INTO debt(date_debt, id_apartament, debt)
SELECT date_debt, id_apartament, debt
FROM debt_temp;

DROP TABLE debt_temp;
UPDATE version SET version = '01.07.006';
