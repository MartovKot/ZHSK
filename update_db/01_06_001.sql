ALTER TABLE bay RENAME TO bay_temp;

CREATE TABLE bay (
    id_apartament INTEGER NOT NULL,
    year_bay INTEGER NOT NULL,
    month_bay TEXT NOT NULL,
    day_bay TEXT NOT NULL,
    bay REAL,
    UNIQUE (id_apartament, year_bay, month_bay, day_bay)
);

REPLACE INTO bay (id_apartament, year_bay, month_bay, day_bay,bay)
    SELECT id_apartament, year_bay, month_bay, day_bay,bay FROM bay_temp;


REPLACE INTO bay (id_apartament, year_bay, month_bay, day_bay,bay)
    SELECT id_apartament, year_bay, month_bay, day_bay,bay FROM bay_temp;

DROP TABLE bay_temp;

ALTER TABLE bay RENAME TO bay_temp;

--UPDATE bay_temp SET day_bay='0'||day_bay  WHERE day_bay<'10';
--UPDATE bay_temp SET month_bay='0'||month_bay  WHERE month_bay<'10';

CREATE TABLE bay (
    id_apartament INTEGER NOT NULL,
    bay_date INTEGER  NOT NULL,
    bay REAL,
    UNIQUE (id_apartament, bay_date)
);

REPLACE INTO bay (id_apartament, bay_date, bay)
SELECT id_apartament, STRFTIME('%s',bt.year_bay||'-'||bt.month_bay||'-'||bt.day_bay), bay
FROM bay_temp bt ;
