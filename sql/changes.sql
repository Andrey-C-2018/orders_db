USE orders;

DROP TABLE IF EXISTS people;
CREATE TABLE people(
	id_person INTEGER AUTO_INCREMENT PRIMARY KEY,
	name VARCHAR(255) NOT NULL,
	name_short VARCHAR(200) NOT NULL,
	bdate DATE,
	last_changed TIMESTAMP DEFAULT current_timestamp() ON UPDATE current_timestamp(),
	temp_id INTEGER NOT NULL UNIQUE);

START TRANSACTION;
INSERT INTO people(temp_id, name, name_short, bdate) 
	SELECT id_advocat, adv_name, adv_name_short, adv_bdate FROM advocats;

ALTER table advocats DROP COLUMN adv_name;
ALTER table advocats DROP COLUMN adv_name_short;
ALTER table advocats DROP COLUMN adv_bdate;
ALTER table advocats DROP COLUMN mark;

UPDATE advocats b, people p SET b.id_advocat = p.id_person WHERE p.temp_id = b.id_advocat;
ALTER table people DROP COLUMN temp_id;
ALTER table advocats ADD FOREIGN KEY fkey_people(id_advocat) REFERENCES people(id_person) 
	ON UPDATE CASCADE ON DELETE CASCADE;
COMMIT;

ALTER table orders DROP FOREIGN KEY `orders_ibfk_1`;
ALTER TABLE orders RENAME COLUMN id_adv TO id_defender;
#ALTER TABLE orders CHANGE COLUMN id_adv id_defender INTEGER(10);
ALTER table orders ADD FOREIGN KEY fkey_defender(id_defender) REFERENCES people(id_person) 
	ON UPDATE CASCADE ON DELETE RESTRICT;

DROP TABLE IF EXISTS psychologists;
CREATE TABLE psychologists(
	id_psycho INTEGER NOT NULL PRIMARY KEY,
	address VARCHAR(255),
	tel VARCHAR(80),
	email VARCHAR(100),
	last_changed TIMESTAMP DEFAULT current_timestamp() ON UPDATE current_timestamp());

START TRANSACTION;
INSERT INTO psychologists(id_psycho, address, tel, email) 
	SELECT id_advocat, address, tel, email FROM advocats
	WHERE license_no = '0';

DELETE FROM advocats WHERE license_no = '0';
COMMIT;

GRANT SELECT,INSERT,UPDATE,DELETE ON `orders`.`people` TO `orders_user_new`@`%`;
GRANT SELECT,INSERT,UPDATE,DELETE ON `orders`.`advocats` TO `orders_user_new`@`%`;
GRANT SELECT,INSERT,UPDATE,DELETE ON `orders`.`psychologists` TO `orders_user_new`@`%`;

UPDATE parameters SET search_form_version='1.1.0', acts_form_version='1.1.0' WHERE version='100';