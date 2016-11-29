LOAD xsmall FROM 'xsmall.del' WITH INDEX
SELECT * FROM xsmall WHERE key>1578 AND value>='G.I. Blues'
