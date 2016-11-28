LOAD xlarge FROM 'xlarge.del' WITH INDEX
SELECT COUNT(*) FROM xlarge
SELECT * FROM xlarge WHERE key = 4240
SELECT * FROM xlarge WHERE key > 400 AND key < 500 AND key > 100 AND key < 4000000
