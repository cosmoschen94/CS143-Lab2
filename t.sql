LOAD small FROM 'small.del' WITH INDEX
SELECT COUNT(*) FROM small
SELECT * FROM small WHERE key > 100 AND key < 500
