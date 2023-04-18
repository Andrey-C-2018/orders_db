ALTER TABLE `orders`.`payments` 
DROP FOREIGN KEY `payments_ibfk_7`,
DROP FOREIGN KEY `payments_ibfk_6`,
DROP FOREIGN KEY `payments_ibfk_5`,
DROP FOREIGN KEY `payments_ibfk_4`,
DROP FOREIGN KEY `payments_ibfk_3`;

ALTER TABLE `orders`.`payments` 
DROP INDEX `id_checker` ,
DROP INDEX `id_user` ,
DROP INDEX `id_informer` ,
DROP INDEX `id_stage` ;

ALTER TABLE `orders`.`payments` ADD COLUMN `rec_id` INT NOT NULL AUTO_INCREMENT AFTER `cancel_med`, DROP PRIMARY KEY, ADD PRIMARY KEY (`rec_id`);

CREATE UNIQUE INDEX payments_pk_alt ON payments(id_center,id_order,order_date,id_stage,act_no,cycle);
ALTER TABLE payments ADD FOREIGN KEY (id_stage) REFERENCES stages(id_st) ON UPDATE CASCADE;
ALTER TABLE payments ADD FOREIGN KEY (id_informer) REFERENCES informers (id_inf) ON UPDATE CASCADE;
ALTER TABLE payments ADD FOREIGN KEY (id_center, id_order, order_date) REFERENCES orders (id_center_legalaid, id, order_date) ON DELETE CASCADE ON UPDATE CASCADE;
ALTER TABLE payments ADD FOREIGN KEY (id_user) REFERENCES users (id_user) ON DELETE SET NULL ON UPDATE CASCADE;
ALTER TABLE payments ADD FOREIGN KEY (id_checker) REFERENCES passwords (id_user) ON UPDATE CASCADE;

UPDATE `orders`.`parameters` SET `search_form_version`='1.0.18', `acts_form_version`='1.0.23' WHERE `version`='100';