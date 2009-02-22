
CREATE TABLE qws_config (id INTEGER PRIMARY KEY AUTOINCREMENT, conf_key TEXT, conf_value TEXT);
CREATE TABLE qws_news (id INTEGER PRIMARY KEY AUTOINCREMENT, news_username TEXT, news_date TEXT, news_text TEXT);
CREATE TABLE qws_files (id INTEGER PRIMARY KEY AUTOINCREMENT, file_path TEXT, file_type INTEGER, file_comment TEXT);
CREATE TABLE qws_groups (id INTEGER PRIMARY KEY AUTOINCREMENT, group_name TEXT, group_privs TEXT);
CREATE TABLE qws_accounts (id INTEGER PRIMARY KEY AUTOINCREMENT, acc_name TEXT, acc_secret TEXT, acc_logincount INTEGER, acc_privileges TEXT, acc_group TEXT);

INSERT INTO "qws_accounts" VALUES(1,'admin','21232f297a57a5a743894a0e4a801fc3',NULL,'aBbPtCAfRrdMEiKpuUD','');


