CREATE TABLE settings (
            name_setting TEXT,
            value_setting TEXT,
            UNIQUE (name_setting)
);

UPDATE version SET version = '01.06.008';
