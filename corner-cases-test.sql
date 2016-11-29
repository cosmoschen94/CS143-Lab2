LOAD xsmall FROM 'xsmall.del' WITH INDEX
SELECT * FROM xsmall WHERE value='King Creole'
SELECT * FROM xsmall WHERE value<>'King Creole'
SELECT * FROM xsmall WHERE value<'King Creole'
SELECT * FROM xsmall WHERE value>'King Creole'
SELECT * FROM xsmall WHERE value>='King Creole'
SELECT * FROM xsmall WHERE value<='King Creole'
SELECT * FROM xsmall WHERE key=2244 AND key<2244
SELECT * FROM xsmall WHERE key>=2244 AND key<=2244
SELECT * FROM xsmall WHERE key=2244 AND key>2244
SELECT * FROM xsmall WHERE key<2244 AND key>2244
SELECT * FROM xsmall WHERE key>2244 AND key<2244
SELECT * FROM xsmall WHERE key=2244 AND key=2244
SELECT * FROM xsmall WHERE key<>2244 AND key<>2244
SELECT * FROM xsmall WHERE key=2244 AND key<1639
SELECT * FROM xsmall WHERE key>=2244 AND key<=1639
SELECT * FROM xsmall WHERE key=2244 AND key>1639
SELECT * FROM xsmall WHERE key<2244 AND key>1639
SELECT * FROM xsmall WHERE key>2244 AND key<1639
SELECT * FROM xsmall WHERE key=2244 AND key=1639
SELECT * FROM xsmall WHERE key<>2244 AND key<>1639
SELECT * FROM xsmall WHERE key<300 AND key<3000 AND key<30000
SELECT * FROM xsmall WHERE key>271 AND key<=272 AND key<300000
SELECT * FROM xsmall WHERE key=272 AND key<>272
SELECT * FROM xsmall WHERE key=272 AND value<>"Superman"
SELECT * FROM xsmall WHERE key=272 AND value<>"Baby Take a Bow"

LOAD small FROM 'small.del' WITH INDEX
SELECT * FROM small WHERE value='King Creole'
SELECT * FROM small WHERE value<>'King Creole'
SELECT * FROM small WHERE value<'King Creole'
SELECT * FROM small WHERE value>'King Creole'
SELECT * FROM small WHERE value>='King Creole'
SELECT * FROM small WHERE value<='King Creole'
SELECT * FROM small WHERE key=2244 AND key<2244
SELECT * FROM small WHERE key>=2244 AND key<=2244
SELECT * FROM small WHERE key=2244 AND key>2244
SELECT * FROM small WHERE key<2244 AND key>2244
SELECT * FROM small WHERE key>2244 AND key<2244
SELECT * FROM small WHERE key=2244 AND key=2244
SELECT * FROM small WHERE key<>2244 AND key<>2244
SELECT * FROM small WHERE key=2244 AND key<1639
SELECT * FROM small WHERE key>=2244 AND key<=1639
SELECT * FROM small WHERE key=2244 AND key>1639
SELECT * FROM small WHERE key<2244 AND key>1639
SELECT * FROM small WHERE key>2244 AND key<1639
SELECT * FROM small WHERE key=2244 AND key=1639
SELECT * FROM small WHERE key<>2244 AND key<>1639
SELECT * FROM small WHERE key<300 AND key<3000 AND key<30000
SELECT * FROM small WHERE key>271 AND key<=272 AND key<300000
SELECT * FROM small WHERE key=272 AND key<>272
SELECT * FROM small WHERE key=272 AND value<>'Baby Take a Bow'
-- doesnt work:
SELECT * FROM small WHERE key=272 AND value>'Superman'
SELECT * FROM small WHERE key=272 AND value='Superman'


LOAD medium FROM 'medium.del' WITH INDEX
SELECT * FROM medium WHERE value='King Creole'
SELECT * FROM medium WHERE value<>'King Creole'
SELECT * FROM medium WHERE value<'King Creole'
SELECT * FROM medium WHERE value>'King Creole'
SELECT * FROM medium WHERE value>='King Creole'
SELECT * FROM medium WHERE value<='King Creole'
SELECT * FROM medium WHERE key=2244 AND key<2244
SELECT * FROM medium WHERE key>=2244 AND key<=2244
SELECT * FROM medium WHERE key=2244 AND key>2244
SELECT * FROM medium WHERE key<2244 AND key>2244
SELECT * FROM medium WHERE key>2244 AND key<2244
SELECT * FROM medium WHERE key=2244 AND key=2244
SELECT * FROM medium WHERE key<>2244 AND key<>2244
SELECT * FROM medium WHERE key=2244 AND key<1639
SELECT * FROM medium WHERE key>=2244 AND key<=1639
SELECT * FROM medium WHERE key=2244 AND key>1639
SELECT * FROM medium WHERE key<2244 AND key>1639
SELECT * FROM medium WHERE key>2244 AND key<1639
SELECT * FROM medium WHERE key=2244 AND key=1639
SELECT * FROM medium WHERE key<>2244 AND key<>1639
SELECT * FROM medium WHERE key<300 AND key<3000 AND key<30000
SELECT * FROM medium WHERE key>271 AND key<=272 AND key<300000
SELECT * FROM medium WHERE key=272 AND key<>272
SELECT * FROM medium WHERE key=272 AND value<>"Superman"
SELECT * FROM medium WHERE key=272 AND value<>"Baby Take a Bow"

LOAD large FROM 'large.del' WITH INDEX
SELECT * FROM large WHERE value='King Creole'
SELECT * FROM large WHERE value<>'King Creole'
SELECT * FROM large WHERE value<'King Creole'
SELECT * FROM large WHERE value>'King Creole'
SELECT * FROM large WHERE value>='King Creole'
SELECT * FROM large WHERE value<='King Creole'
SELECT * FROM large WHERE key=2244 AND key<2244
SELECT * FROM large WHERE key>=2244 AND key<=2244
SELECT * FROM large WHERE key=2244 AND key>2244
SELECT * FROM large WHERE key<2244 AND key>2244
SELECT * FROM large WHERE key>2244 AND key<2244
SELECT * FROM large WHERE key=2244 AND key=2244
SELECT * FROM large WHERE key<>2244 AND key<>2244
SELECT * FROM large WHERE key=2244 AND key<1639
SELECT * FROM large WHERE key>=2244 AND key<=1639
SELECT * FROM large WHERE key=2244 AND key>1639
SELECT * FROM large WHERE key<2244 AND key>1639
SELECT * FROM large WHERE key>2244 AND key<1639
SELECT * FROM large WHERE key=2244 AND key=1639
SELECT * FROM large WHERE key<>2244 AND key<>1639
SELECT * FROM large WHERE key<300 AND key<3000 AND key<30000
SELECT * FROM large WHERE key>271 AND key<=272 AND key<300000
SELECT * FROM large WHERE key=272 AND key<>272
SELECT * FROM large WHERE key=272 AND value<>"Superman"
SELECT * FROM large WHERE key=272 AND value<>"Baby Take a Bow"

LOAD xlarge FROM 'xlarge.del' WITH INDEX
SELECT * FROM xlarge WHERE value='King Creole'
SELECT * FROM xlarge WHERE value<>'King Creole'
SELECT * FROM xlarge WHERE value<'King Creole'
SELECT * FROM xlarge WHERE value>'King Creole'
SELECT * FROM xlarge WHERE value>='King Creole'
SELECT * FROM xlarge WHERE value<='King Creole'
SELECT * FROM xlarge WHERE key=2244 AND key<2244
SELECT * FROM xlarge WHERE key>=2244 AND key<=2244
SELECT * FROM xlarge WHERE key=2244 AND key>2244
SELECT * FROM xlarge WHERE key<2244 AND key>2244
SELECT * FROM xlarge WHERE key>2244 AND key<2244
SELECT * FROM xlarge WHERE key=2244 AND key=2244
SELECT * FROM xlarge WHERE key<>2244 AND key<>2244
SELECT * FROM xlarge WHERE key=2244 AND key<1639
SELECT * FROM xlarge WHERE key>=2244 AND key<=1639
SELECT * FROM xlarge WHERE key=2244 AND key>1639
SELECT * FROM xlarge WHERE key<2244 AND key>1639
SELECT * FROM xlarge WHERE key>2244 AND key<1639
SELECT * FROM xlarge WHERE key=2244 AND key=1639
SELECT * FROM xlarge WHERE key<>2244 AND key<>1639
SELECT * FROM xlarge WHERE key<300 AND key<3000 AND key<30000
SELECT * FROM xlarge WHERE key>271 AND key<=272 AND key<300000
SELECT * FROM xlarge WHERE key=272 AND key<>272
SELECT * FROM xlarge WHERE key=272 AND value<>"Superman"
SELECT * FROM xlarge WHERE key=272 AND value<>"Baby Take a Bow"
