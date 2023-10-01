ALTER table `orders`.`payments` ADD COLUMN rejection_date DATE AFTER cancel_med;
ALTER table `orders`.`payments` ADD COLUMN rejection_reason VARCHAR(120) AFTER rejection_date;