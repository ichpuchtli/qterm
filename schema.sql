drop table if exists tags;
create table tags (
  id integer primary key autoincrement,
  epoch integer not null default (strftime('%s', 'now')),
  utc timestamp not null default (datetime('now')),
  localtime timestamp not null default (datetime('now', 'localtime')),
  tag text not null
);
