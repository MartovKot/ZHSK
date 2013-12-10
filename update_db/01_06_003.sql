CREATE TABLE counter_value {
    id_counter_value INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    id_list_app_usluga INTEGER,
    date_counter_value INTEGER,
    value_home INTEGER,
    value_end INTEGER,
    UNIQUE (id_list_app_usluga, date_counter_value)
};


CREATE TABLE pokazanie (
            id_pokazanie INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
            id_list_app_usluga INTEGER,
            date_month INTEGER,
            date_year INTEGER,
            pokazanie_home INTEGER,
            pokazanie_end INTEGER,
            UNIQUE (id_list_app_usluga, date_month, date_year)
);
