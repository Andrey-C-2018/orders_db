USE orders;

ALTER TABLE `orders`.`informers` DROP FOREIGN KEY `informers_ibfk_1`;

ALTER TABLE `orders`.`informers` CHANGE COLUMN `id_distr` `id_location` INT(10) NULL DEFAULT NULL ;

ALTER TABLE `orders`.`informers` ADD CONSTRAINT `informers_ibfk_1`
  FOREIGN KEY (`id_location`)
  REFERENCES `orders`.`districts` (`id_distr`)
  ON UPDATE CASCADE;
