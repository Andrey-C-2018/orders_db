ALTER TABLE `orders`.`payments` 
ADD COLUMN `appeal_softer` TINYINT(1) NULL AFTER `outg_daynight`,
ADD COLUMN `detect_softer` TINYINT(1) NULL AFTER `appeal_softer`,
ADD COLUMN `reject_appeal` TINYINT(1) NULL AFTER `detect_softer`,
ADD COLUMN `change_kval_kr` TINYINT(1) NULL AFTER `reject_appeal`,
ADD COLUMN `reduce_ep` TINYINT(1) NULL AFTER `change_kval_kr`,
ADD COLUMN `change_med` TINYINT(1) NULL AFTER `reduce_ep`,
ADD COLUMN `cancel_med` TINYINT(1) NULL AFTER `change_med`;