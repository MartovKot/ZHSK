CREATE TABLE debt_t (
            id_debt INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            year_debt INTEGER,
            month_debt TEXT,
            id_apartament INTEGER,
            debt REAL,
            UNIQUE (year_debt,month_debt,id_apartament)
);

REPLACE INTO debt_t(id_debt, year_debt, month_debt, id_apartament, debt)
SELECT id_debt, year_debt, month_debt, id_apartament, debt
FROM debt WHERE debt.month_debt>=10;

REPLACE INTO debt_t(id_debt, year_debt, month_debt, id_apartament, debt)
SELECT id_debt, year_debt, '0'||month_debt, id_apartament, debt
FROM debt WHERE debt.month_debt<10;

DROP TABLE debt;

CREATE TABLE debt (
            id_debt INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            date_debt INTEGER,
            id_apartament INTEGER,
            debt REAL,
            UNIQUE (date_debt,id_apartament)
);

REPLACE INTO debt(id_debt, date_debt, id_apartament, debt)
SELECT id_debt, STRFTIME('%s',t.year_debt||'-'||t.month_debt||'-01'), id_apartament, debt
FROM debt_t t;

DROP TABLE debt_t;

UPDATE version SET version = '01.06.004';
