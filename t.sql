LOAD xsmall FROM 'xsmall.del' WITH INDEX
SELECT COUNT(*) FROM xsmall
SELECT * FROM xsmall WHERE key < 2500
