ALTER TABLE `orders`.`payments` 
ADD COLUMN `act_no` INT NULL AFTER `id_act`;

UPDATE `orders`.`payments` SET act_no = 1;

ALTER TABLE `orders`.`payments` 
CHANGE COLUMN `act_no` `act_no` INT(11) NOT NULL DEFAULT 0 ,
DROP PRIMARY KEY,
ADD PRIMARY KEY (`id_center`, `id_order`, `order_date`, `id_stage`, `cycle`, `act_no`);
