-- LOAD xsmall FROM 'xsmall.del' WITH INDEX
-- SELECT * FROM xsmall WHERE value='King Creole'
-- SELECT * FROM xsmall WHERE value<>'King Creole'
-- SELECT * FROM xsmall WHERE value<'King Creole'
-- SELECT * FROM xsmall WHERE value>'King Creole'
-- SELECT * FROM xsmall WHERE value>='King Creole'
-- SELECT * FROM xsmall WHERE value<='King Creole'
-- SELECT * FROM xsmall WHERE key=2244 AND key<2244
-- SELECT * FROM xsmall WHERE key>=2244 AND key<=2244
-- SELECT * FROM xsmall WHERE key=2244 AND key>2244
-- SELECT * FROM xsmall WHERE key<2244 AND key>2244
-- SELECT * FROM xsmall WHERE key>2244 AND key<2244
-- SELECT * FROM xsmall WHERE key=2244 AND key=2244
-- SELECT * FROM xsmall WHERE key<300 AND key<3000 AND key<30000
-- SELECT * FROM xsmall WHERE key>271 AND key<=272 AND key<300000
-- SELECT * FROM xsmall WHERE key=272 AND key<>272
-- SELECT * FROM xsmall WHERE key=272 AND value<>"Superman"
-- SELECT * FROM xsmall WHERE key=272 AND value<>"Baby Take a Bow"

LOAD xsmall FROM 'xsmall.del' WITH INDEX
SELECT COUNT(*) FROM xsmall
SELECT * FROM xsmall WHERE key < 2500

LOAD small FROM 'small.del' WITH INDEX
SELECT COUNT(*) FROM small
SELECT * FROM small WHERE key > 100 AND key < 500

LOAD medium FROM 'medium.del' WITH INDEX
SELECT COUNT(*) FROM medium
SELECT * FROM medium WHERE key = 489
