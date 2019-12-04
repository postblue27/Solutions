SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

USE [master];
GO

IF EXISTS (SELECT * FROM sys.databases WHERE name = 'DataEntryDB')
	DROP DATABASE DataEntryDB;
GO

-- Create the StudentMgmtSystem database.
CREATE DATABASE DataEntryDB;
GO

-- Specify a simple recovery model 
-- to keep the log growth to a minimum.
ALTER DATABASE DataEntryDB 
	SET RECOVERY SIMPLE;
GO

USE DataEntryDB;
GO

-- Create the customer table.
IF NOT EXISTS (SELECT * FROM sys.objects 
		WHERE object_id = OBJECT_ID(N'[dbo].[customer]') 
		AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[customer](
	[cus_code] [int] NOT NULL,
	[cus_lname] [varchar](20) NOT NULL,
	[cus_fname] [varchar](20) NOT NULL,
	[cus_initial] [char] NULL,
	[cus_areacode] [int] NULL,
	[cus_phone] [int] NULL,
	CONSTRAINT [PK_customer] PRIMARY KEY CLUSTERED ([cus_code])
)
END
GO

-- Create the invoice table.
IF NOT EXISTS (SELECT * FROM sys.objects 
		WHERE object_id = OBJECT_ID(N'[dbo].[invoice]') 
		AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[invoice](
	[inv_number] [int] NOT NULL,
	[cus_code] [int] NOT NULL,
	[inv_date] [date] NOT NULL,
	CONSTRAINT [PK_invoice] PRIMARY KEY CLUSTERED ([inv_number]),
	CONSTRAINT [FK_invoice_customer] FOREIGN KEY ([cus_code])
	REFERENCES [dbo].[customer] ([cus_code])
)
END
GO

-- Create the vendor table.
IF NOT EXISTS (SELECT * FROM sys.objects 
		WHERE object_id = OBJECT_ID(N'[dbo].[vendor]') 
		AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[vendor](
	[vend_code] [int] NOT NULL,
	[vend_name] [varchar](30) NOT NULL,
	[vend_contact] [varchar](30) NOT NULL,
	[vend_areacode] [int] NOT NULL,
	[vend_phone] [int] NOT NULL,
	CONSTRAINT [PK_vendor] PRIMARY KEY CLUSTERED ([vend_code])
)
END
GO

-- Create the product table.
IF NOT EXISTS (SELECT * FROM sys.objects 
		WHERE object_id = OBJECT_ID(N'[dbo].[product]') 
		AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[product](
	[prod_code] [int] NOT NULL,
	[prod_desc] [varchar](50) NOT NULL,
	[prod_price] [int] NOT NULL,
	[prod_quant] [int] NOT NULL,
	[vend_code] [int] NOT NULL,
	CONSTRAINT [PK_product] PRIMARY KEY CLUSTERED ([prod_code]),
	CONSTRAINT [FK_product_vendor] FOREIGN KEY ([vend_code])
	REFERENCES [dbo].[vendor] ([vend_code])
)
END
GO

-- Create the line table.
IF NOT EXISTS (SELECT * FROM sys.objects 
		WHERE object_id = OBJECT_ID(N'[dbo].[line]') 
		AND type in (N'U'))
BEGIN
CREATE TABLE [dbo].[line](
	[inv_number] [int] NOT NULL,
	[prod_code] [int] NOT NULL,
	[line_units] [int] NOT NULL,
	CONSTRAINT [FK_line_invoice] FOREIGN KEY ([inv_number])
	REFERENCES [dbo].[invoice] ([inv_number]),
	CONSTRAINT [FK_line_product] FOREIGN KEY ([prod_code])
	REFERENCES [dbo].[product] ([prod_code])
)
END
GO

INSERT INTO customer VALUES 
	(10010, 'Johnson', 'Alfred', 'A', 615, 8442573),
	(10011, 'Dunne', 'Leona', 'K', 713, 8941238),
	(10012, 'Smith', 'Walter', 'W', 615, 8942285),
	(10013, 'Roberts', 'Paul', 'F', 615, 2221672),
	(10014, 'Orlando', 'Myla', NULL, 615, 2971228)

GO

INSERT INTO invoice VALUES 
	(1001, 10011, '2008-08-03'),
	(1002, 10014, '2008-08-04'),
	(1003, 10012, '2008-03-20'),
	(1004, 10014, '2008-09-23')

GO

INSERT INTO vendor VALUES 
	(232, 'Bryson', 'Smith', 615, 2233234),
	(235, 'Walls', 'Anderson', 615, 2158995),
	(236, 'Jason', 'Schmidt', 651, 2468850)

GO

INSERT INTO product	VALUES 
	(12321, 'hammer', 189 ,20, 232),
	(65781, 'chain', 12, 45, 235),
	(34256, 'tape', 35, 60, 236),
	(12333, 'hanger', 200 ,10, 232)

GO

INSERT INTO line VALUES 
	(1001,  12321, 1),
	(1001,  65781, 3),
	(1002,  34256, 6),
	(1003,  12321, 5),
	(1002,  12321, 6)

GO
INSERT INTO vendor VALUES 
	(231, 'Adam', 'Eric', 615, 2158995)

GO
INSERT INTO product VALUES 
	(12322, 'coil', 189, 20, 231)


GO
SELECT cus_code, cus_lname, cus_fname FROM customer

GO
SELECT inv_number, inv_date FROM invoice, customer 
WHERE invoice.cus_code = customer.cus_code AND customer.cus_code = 10014

GO
SELECT product.prod_code, product.prod_quant FROM product, invoice, line
WHERE product.prod_code = line.prod_code AND line.inv_number = invoice.inv_number 
AND invoice.inv_number = 1001

GO
SELECT product.prod_desc, product.prod_price FROM product, vendor
WHERE product.vend_code = vendor.vend_code AND vendor.vend_contact = 'Nobody'

GO
SELECT product.prod_desc, vendor.vend_name FROM product, vendor
WHERE product.vend_code = vendor.vend_code AND product.prod_quant <= 60

GO
SELECT product.prod_desc, customer.cus_fname, customer.cus_lname 
FROM product, line, invoice, customer
WHERE product.prod_code = line.prod_code AND line.inv_number = invoice.inv_number 
AND invoice.cus_code = customer.cus_code 