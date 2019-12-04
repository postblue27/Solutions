--1
GO
SELECT COUNT(*) AS count_vend FROM (SELECT DISTINCT vendor.vend_code FROM vendor, product 
WHERE vendor.vend_code = product.vend_code AND product.prod_price < 185) some_table

--2
GO 
SELECT vendor.vend_code, vendor.vend_name, product.prod_desc, product.prod_quant
FROM vendor, product
WHERE product.vend_code = vendor.vend_code AND product.prod_quant = 
	(SELECT MIN(p.prod_quant) FROM product p WHERE p.vend_code = vendor.vend_code)

--3
GO
SELECT invoice.inv_number, count(*) FROM product, invoice, line WHERE line.prod_code = product.prod_code
	AND line.inv_number = invoice.inv_number GROUP BY invoice.inv_number

--4
GO
SELECT customer.cus_code, count(*) AS num_of_invoices FROM customer, invoice 
WHERE invoice.cus_code = customer.cus_code GROUP BY customer.cus_code

--5
GO
SELECT sum(product.prod_quant*product.prod_price) FROM product

--6
GO
SELECT vendor.vend_code, vendor.vend_contact, count(*) AS num_of_products
FROM vendor, product 
WHERE product.vend_code = vendor.vend_code GROUP BY vendor.vend_code, vendor.vend_contact

--7
GO
SELECT product.prod_desc, product.prod_price, product.vend_code 
FROM product 
WHERE product.prod_price =
(SELECT min(p.prod_price) FROM product p)

--8
GO
SELECT invoice.inv_number, sum(line.line_units*product.prod_price)
FROM invoice, line, product
WHERE line.inv_number = invoice.inv_number AND line.prod_code = product.prod_code
GROUP BY invoice.inv_number

--9
GO
SELECT customer.cus_code, customer.cus_fname, customer.cus_lname, count(*) AS products_bought
FROM invoice, line, product, customer
WHERE customer.cus_code = invoice.cus_code AND line.prod_code = product.prod_code 
AND line.inv_number = invoice.inv_number 
GROUP BY customer.cus_code, customer.cus_fname, customer.cus_lname