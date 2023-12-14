USE orders;

START TRANSACTION;
DROP TABLE IF EXISTS temp_informers; 
CREATE TEMPORARY TABLE temp_informers(
  id_inf INTEGER NOT NULL PRIMARY KEY,
  informer_name VARCHAR(100) NOT NULL,
  informer_full_name VARCHAR(100) NOT NULL,
  id_location INTEGER NOT NULL,
  inf_class VARCHAR(40));

ALTER TABLE payments DROP FOREIGN KEY `payments_ibfk_2`;
ALTER TABLE payments DROP INDEX `id_informer`;

INSERT INTO temp_informers SELECT * FROM informers;
DELETE FROM informers;
ALTER TABLE informers ADD COLUMN temp_id INT(10) NOT NULL;
ALTER TABLE informers CHANGE COLUMN `id_inf` `id_inf` INT(10) NOT NULL AUTO_INCREMENT,
  DROP PRIMARY KEY, ADD PRIMARY KEY (`id_inf`);

INSERT INTO informers(temp_id, informer_name, informer_full_name, id_distr, inf_class) 
  SELECT id_inf, informer_name, informer_full_name, id_location, inf_class 
  FROM temp_informers
  ORDER BY informer_name;
DROP TABLE temp_informers;

UPDATE payments aa, informers inf
  SET aa.id_informer = inf.id_inf WHERE aa.id_informer = inf.temp_id; 
ALTER TABLE informers DROP COLUMN temp_id;

ALTER TABLE payments ADD FOREIGN KEY fkey_informer(id_informer) REFERENCES informers(id_inf) 
  ON UPDATE CASCADE ON DELETE RESTRICT;
COMMIT;

ALTER TABLE informers DROP FOREIGN KEY `informers_ibfk_1`;
ALTER TABLE informers CHANGE COLUMN `id_distr` `id_location` INT(10) NULL DEFAULT NULL ;
ALTER TABLE informers ADD CONSTRAINT `informers_ibfk_1`
  FOREIGN KEY (id_location)
  REFERENCES districts (id_distr)
  ON UPDATE CASCADE ON DELETE RESTRICT;
