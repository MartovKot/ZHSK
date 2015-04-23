ALTER TABLE credited_of_apartament RENAME TO credited_of_apartament_temp;
CREATE TABLE credited_of_apartament (
            id_apartament INTEGER NOT NULL,
            date_credited_of_apartament INTEGER NOT NULL,
            credited_with_counter REAL DEFAULT 0.0,
            credited_out_counter REAL DEFAULT 0.0,
            UNIQUE (id_apartament,date_credited_of_apartament)
);
INSERT OR REPLACE INTO credited_of_apartament( id_apartament,date_credited_of_apartament, credited_with_counter, credited_out_counter )
SELECT  id_apartament,date_credited_of_apartament, credited_with_counter, credited_out_counter
FROM credited_of_apartament_temp ;
DROP TABLE credited_of_apartament_temp;
UPDATE version SET version = '01.07.005';
