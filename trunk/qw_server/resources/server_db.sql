
CREATE TABLE qws_config (id INTEGER PRIMARY KEY AUTOINCREMENT, conf_key TEXT, conf_value TEXT);
CREATE TABLE qws_news (id INTEGER PRIMARY KEY AUTOINCREMENT, news_username TEXT, news_date TEXT, news_text TEXT);

INSERT INTO qws_news VALUES (1, 'Basti', '2009-02-22T22:00:00', 'Welcome to Qwired Server!

Qwired Server is server daemon for the Wired protocol and offers pretty features such as a plugin backend, protocol extensions for Qwired clients and much more.

Enjoy! :)

Basti');


CREATE TABLE qws_files (id INTEGER PRIMARY KEY AUTOINCREMENT, file_path TEXT, file_type INTEGER, file_comment TEXT);
CREATE TABLE qws_groups (id INTEGER PRIMARY KEY AUTOINCREMENT, group_name TEXT, group_privs TEXT);
CREATE TABLE qws_accounts (id INTEGER PRIMARY KEY AUTOINCREMENT, acc_name TEXT, acc_secret TEXT, acc_logincount INTEGER, acc_privileges TEXT, acc_group TEXT, acc_maxupload INTEGER, acc_maxdownload INTEGER, acc_speedupload INTEGER, acc_speeddownload INTEGER);

INSERT INTO "qws_accounts" VALUES (1, 'guest', '', 0, 'ipdut', '', 0, 0, 0, 0);
INSERT INTO "qws_accounts" VALUES (2, 'admin', 'd033e22ae348aeb5660fc2140aec35850c4da997', 0, 'aBbPtCAfRrdMEiKpuUD', '', 0, 0, 0, 0);


