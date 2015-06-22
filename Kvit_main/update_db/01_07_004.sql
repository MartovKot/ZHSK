ALTER TABLE credited RENAME TO credited_temp;
CREATE TABLE credited (
            id_list_app_usluga INTEGER,
            date_credited INTEGER,
            credited_of_service REAL,
        UNIQUE (id_list_app_usluga, date_credited)
);
INSERT OR REPLACE INTO credited(id_list_app_usluga, date_credited, credited_of_service )
SELECT id_list_app_usluga, date_credited, credited_of_service
FROM credited_temp ;
DROP TABLE credited_temp;
UPDATE version SET version = '01.07.004';
